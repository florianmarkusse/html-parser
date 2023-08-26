#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/dom/dom-registry.h"
#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/data/definitions.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/utils/print/error.h"
#include "flo/html-parser/utils/text/text.h"

typedef enum {
    BASIC_CONTEXT,
    SCRIPT_CONTEXT,
    STYLE_CONTEXT,
    ROGUE_OPEN_TAG
} TextParsing;

DomStatus getNewNodeID(node_id *currentNodeID, const NodeType nodeType,
                       node_id *prevNodeID, Dom *dom) {
    *prevNodeID = *currentNodeID;
    return createNode(currentNodeID, nodeType, dom);
}

typedef struct {
    node_id stack[MAX_NODE_DEPTH];
    size_t len;
} __attribute__((aligned(128))) NodeDepth;

DomStatus updateReferences(const node_id newNodeID,
                           const node_id previousNodeID,
                           const NodeDepth *depthStack, Dom *dom) {
    DomStatus documentStatus = DOM_SUCCESS;

    if (newNodeID > 0 && previousNodeID > 0) {
        if (depthStack->len == 0) {
            if ((documentStatus = addNextNode(previousNodeID, newNodeID,
                                              dom)) != DOM_SUCCESS) {
                return documentStatus;
            }
        } else {
            const unsigned int parentNodeID =
                depthStack->stack[depthStack->len - 1];
            if ((documentStatus = addParentChild(parentNodeID, newNodeID,
                                                 dom)) != DOM_SUCCESS) {
                return documentStatus;
            }
            if (parentNodeID == previousNodeID) {
                if ((documentStatus = addParentFirstChild(
                         parentNodeID, newNodeID, dom)) != DOM_SUCCESS) {
                    return documentStatus;
                }
            } else {
                if ((documentStatus = addNextNode(previousNodeID, newNodeID,
                                                  dom)) != DOM_SUCCESS) {
                    return documentStatus;
                }
            }
        }
    }

    return documentStatus;
}

DomStatus addTagToNodeID(const char *htmlString, const size_t elementStartIndex,
                         const size_t elementLen, const node_id nodeID,
                         const bool isPaired, Dom *dom,
                         DataContainer *dataContainer) {
    DomStatus domStatus = DOM_SUCCESS;
    HashElement hashElement;
    indexID newTagID = 0;
    ElementStatus indexStatus =
        newElementToIndex(&dataContainer->tags, &htmlString[elementStartIndex],
                          elementLen, true, &hashElement, &newTagID);

    switch (indexStatus) {
    case ELEMENT_CREATED: {
        if (addTagRegistration(newTagID, isPaired, &hashElement, dom) !=
            DOM_SUCCESS) {
            PRINT_ERROR("Failed to add tag registration.\n");
            return DOM_NO_ELEMENT;
        }
        // Intentional fall through!!!
    }
    case ELEMENT_FOUND: {
        if ((domStatus = setNodeTagID(nodeID, newTagID, dom)) != DOM_SUCCESS) {
            PRINT_ERROR("Failed to set tag ID for new dom node.\n");
            return domStatus;
        }
        break;
    }
    default: {
        ERROR_WITH_CODE_ONLY(elementStatusToString(indexStatus),
                             "Failed to insert into new tag names!\n");
        return DOM_NO_ELEMENT;
    }
    }

    return domStatus;
}

DomStatus parsedomNode(const char *htmlString, size_t *currentPosition,
                       node_id *prevNodeID, node_id *newNodeID,
                       unsigned char *isSingle, TextParsing *context,
                       unsigned char exclamStart, Dom *dom,
                       DataContainer *dataContainer) {
    ElementStatus elementStatus = ELEMENT_SUCCESS;
    DomStatus documentStatus = DOM_SUCCESS;
    char ch = htmlString[++(*currentPosition)];

    if ((documentStatus = getNewNodeID(newNodeID, NODE_TYPE_DOCUMENT,
                                       prevNodeID, dom)) != DOM_SUCCESS) {
        PRINT_ERROR("Failed to create node.\n");
        return documentStatus;
    }

    size_t elementStartIndex = *currentPosition;
    while (ch != ' ' && !isSpecialSpace(ch) && ch != '>' && ch != '\0') {
        ch = htmlString[++(*currentPosition)];
    }
    size_t elementLen = *currentPosition - elementStartIndex;
    if (ch == '\0') {
        elementLen--;
    }

    *isSingle = exclamStart;
    if (ch == '>' && *currentPosition > 0 &&
        htmlString[*currentPosition - 1] == '/') {
        *isSingle = 1;
        elementLen--;
    }

    // Collect attributes here.
    while (ch != '>' && ch != '/' && ch != '\0') {
        while (ch == ' ' || isSpecialSpace(ch)) {
            ch = htmlString[++(*currentPosition)];
        }
        if (ch == '/' || ch == '>') {
            break;
        }

        size_t attrKeyStartIndex = *currentPosition;

        if (ch == '\'' || ch == '"') {
            char quote = ch;
            ch = htmlString[++(*currentPosition)]; // Skip start quote.
            while (ch != quote && ch != '\0') {
                ch = htmlString[++(*currentPosition)];
            }
            ch = htmlString[++(*currentPosition)]; // Skip end quote.
        } else {
            while (ch != ' ' && ch != '>' && ch != '=') {
                ch = htmlString[++(*currentPosition)];
            }
        }
        size_t attrKeyLen = *currentPosition - attrKeyStartIndex;
        if (ch == '>' && *currentPosition > 0 &&
            htmlString[*currentPosition - 1] == '/') {
            *isSingle = 1;
            attrKeyLen--;
        }

        element_id attrKeyID = 0;
        HashElement hashKey;
        if (ch == '=') {
            element_id attrValueID = 0;

            // Expected syntax: key="value" OR
            // Expected syntax: key='value' OR
            // Expected syntax: key=value (This is invalid html, but will still
            // support it) We can do some more interesting stuff but currently
            // not required.
            elementStatus = newElementToIndex(
                &dataContainer->propKeys, &htmlString[attrKeyStartIndex],
                attrKeyLen, true, &hashKey, &attrKeyID);
            if (elementStatus != ELEMENT_FOUND &&
                elementStatus != ELEMENT_CREATED) {
                ERROR_WITH_CODE_ONLY(elementStatusToString(elementStatus),
                                     "Failed to get keyID");
                return DOM_NO_ELEMENT;
            }
            if (elementStatus == ELEMENT_CREATED) {
                if (addPropKeyRegistration(attrKeyID, &hashKey, dom) !=
                    DOM_SUCCESS) {
                    PRINT_ERROR("Failed to add prop key registration.\n");
                    return DOM_NO_ELEMENT;
                }
            }
            ch = htmlString[++(*currentPosition)];

            size_t attrValueStartIndex = *currentPosition;
            if (ch == '\'' || ch == '"') {
                attrValueStartIndex++;
                char quote = ch;
                ch = htmlString[++(*currentPosition)];

                while (ch != quote && ch != '\0') {
                    ch = htmlString[++(*currentPosition)];
                }
            } else {
                while (ch != ' ' && !isSpecialSpace(ch) && ch != '>' &&
                       ch != '\0') {
                    ch = htmlString[++(*currentPosition)];
                }
            }

            size_t attrValueLen = *currentPosition - attrValueStartIndex;
            HashElement hashValue;

            elementStatus = newElementToIndex(
                &dataContainer->propValues, &htmlString[attrValueStartIndex],
                attrValueLen, true, &hashValue, &attrValueID);
            if (elementStatus != ELEMENT_FOUND &&
                elementStatus != ELEMENT_CREATED) {
                ERROR_WITH_CODE_ONLY(elementStatusToString(elementStatus),
                                     "Failed to get value ID");
                return DOM_NO_ELEMENT;
            }
            if (elementStatus == ELEMENT_CREATED) {
                if (addPropValueRegistration(attrValueID, &hashValue, dom) !=
                    DOM_SUCCESS) {
                    PRINT_ERROR("Failed to add prop value registration.\n");
                    return DOM_NO_ELEMENT;
                }
            }

            if ((documentStatus = addProperty(
                     *newNodeID, attrKeyID, attrValueID, dom)) != DOM_SUCCESS) {
                PRINT_ERROR("Failed to add key-value property.\n");
                return DOM_NO_ELEMENT;
            }

            // Move past '"'
            ch = htmlString[++(*currentPosition)];
        } else {
            if (elementToIndex(&dataContainer->propKeys.container,
                               &dataContainer->propKeys.singleLen,
                               &htmlString[attrKeyStartIndex], attrKeyLen, 0, 1,
                               &attrKeyID) != ELEMENT_SUCCESS) {
                PRINT_ERROR("Failed to create element ID for key.\n");
                return DOM_NO_ELEMENT;
            }
            if ((documentStatus = addBooleanProperty(*newNodeID, attrKeyID,
                                                     dom)) != DOM_SUCCESS) {
                PRINT_ERROR("Failed to add boolean property.\n");
                return documentStatus;
            }
        }
    }
    if (ch == '/') {
        *isSingle = 1;
    }

    char tagName[elementLen + 1];
    strncpy(tagName, &htmlString[elementStartIndex], elementLen);
    tagName[elementLen] = '\0';
    *context = BASIC_CONTEXT;
    if (strcmp(tagName, "script") == 0) {
        *context = SCRIPT_CONTEXT;
    }

    if (strcmp(tagName, "style") == 0) {
        *context = STYLE_CONTEXT;
    }

    if ((documentStatus = addTagToNodeID(htmlString, elementStartIndex,
                                         elementLen, *newNodeID, !(*isSingle),
                                         dom, dataContainer)) != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add tag to node ID.\n");
        return documentStatus;
    }

    while (ch != '>' && ch != '\0') {
        ch = htmlString[++(*currentPosition)];
    }
    if (ch != '\0') {
        ch = htmlString[++(*currentPosition)];
    }

    return documentStatus;
}

DomStatus parseBasicdomNode(const char *htmlString, size_t *currentPosition,
                            node_id *prevNodeID, node_id *newNodeID,
                            unsigned char *isSingle, TextParsing *context,
                            Dom *dom, DataContainer *dataContainer) {
    return parsedomNode(htmlString, currentPosition, prevNodeID, newNodeID,
                        isSingle, context, 0, dom, dataContainer);
}

DomStatus parseExclamdomNode(const char *htmlString, size_t *currentPosition,
                             node_id *prevNodeID, node_id *newNodeID, Dom *dom,
                             DataContainer *dataContainer) {
    unsigned char ignore = 0;
    TextParsing ignore2 = BASIC_CONTEXT;
    return parsedomNode(htmlString, currentPosition, prevNodeID, newNodeID,
                        &ignore, &ignore2, 1, dom, dataContainer);
}

unsigned char textNodeAtBasicEnd(const char ch, const char *htmlString,
                                 const size_t currentPosition) {
    return (ch != '\0' && !isSpecialSpace(ch) &&
            (ch != ' ' ||
             (currentPosition > 0 && htmlString[currentPosition - 1] != ' ')));
}

DomStatus parseTextNode(const char *htmlString, size_t *currentPosition,
                        node_id *prevNodeID, node_id *currentNodeID,
                        const element_id textTagID, TextParsing *context,
                        unsigned char *isMerge, Dom *dom,
                        DataContainer *dataContainer) {
    DomStatus documentStatus = DOM_SUCCESS;
    size_t elementStartIndex = *currentPosition;
    char ch = htmlString[*currentPosition];
    // Always consume at least a single character
    size_t elementLen = 0;

    // Continue until we encounter extra space or the end of the text
    // node.

    switch (*context) {
    case BASIC_CONTEXT: {
        while (textNodeAtBasicEnd(ch, htmlString, *currentPosition) &&
               ch != '<') {
            ch = htmlString[++(*currentPosition)];
        }

        elementLen = *currentPosition - elementStartIndex;
        if (*currentPosition > 0 && htmlString[(*currentPosition) - 1] == ' ') {
            if (elementLen > 0) {
                elementLen--;
            }
        }
        break;
    }
    case STYLE_CONTEXT: {
        while (textNodeAtBasicEnd(ch, htmlString, *currentPosition) &&
               strncmp(&htmlString[*currentPosition], "</style",
                       strlen("</style")) != 0) {
            ch = htmlString[++(*currentPosition)];
        }

        elementLen = *currentPosition - elementStartIndex;
        if (*currentPosition > 0 && htmlString[(*currentPosition) - 1] == ' ') {
            if (elementLen > 0) {
                elementLen--;
            }
        }

        if (strncmp(&htmlString[*currentPosition], "</style",
                    strlen("</style")) == 0) {
            *context = BASIC_CONTEXT;
            if (elementLen < 1) {
                return documentStatus;
            }
        }

        break;
    }
    case SCRIPT_CONTEXT: {
        char isInString = 0;
        while (textNodeAtBasicEnd(ch, htmlString, *currentPosition) &&
               (isInString || strncmp(&htmlString[*currentPosition], "</script",
                                      strlen("</script")) != 0)) {
            if (ch == '\'' || ch == '"' || ch == '`') {
                if (isInString == ch) {
                    isInString = 0;
                } else if (!isInString) {
                    isInString = ch;
                }
            }
            ch = htmlString[++(*currentPosition)];
        }

        elementLen = *currentPosition - elementStartIndex;

        if (*currentPosition > 0 && htmlString[(*currentPosition) - 1] == ' ') {
            if (elementLen > 0) {
                elementLen--;
            }
        }
        if (strncmp(&htmlString[*currentPosition], "</script",
                    strlen("</script")) == 0) {
            *context = BASIC_CONTEXT;
            if (elementLen < 1) {
                return documentStatus;
            }
        }

        break;
    }
    case ROGUE_OPEN_TAG: {
        // Always consume the rogue opening tag.
        ch = htmlString[++(*currentPosition)];
        while (textNodeAtBasicEnd(ch, htmlString, *currentPosition) &&
               ch != '<') {
            ch = htmlString[++(*currentPosition)];
        }

        elementLen = *currentPosition - elementStartIndex;
        if (*currentPosition > 0 && htmlString[(*currentPosition) - 1] == ' ') {
            if (elementLen > 0) {
                elementLen--;
            }
        }
        break;
    }
    default: {
        break;
    }
    }

    element_id textID = 0;

    Node prevNode = dom->nodes[*currentNodeID];
    if (prevNode.nodeType == NODE_TYPE_TEXT) {
        *isMerge = 1;
        const char *prevText = getText(prevNode.nodeID, dom, dataContainer);
        const size_t mergedLen = strlen(prevText) + elementLen +
                                 2; // Adding a whitespace in between.

        char buffer[mergedLen];
        strcpy(buffer, prevText);
        strcat(buffer, " ");
        strncat(buffer, &htmlString[elementStartIndex], elementLen);
        buffer[mergedLen - 1] = '\0';

        if (elementToIndex(&dataContainer->text.container,
                           &dataContainer->text.len, buffer, mergedLen, 1, 0,
                           &textID) != ELEMENT_SUCCESS) {
            PRINT_ERROR("Failed to create text ID for merging text nodes.\n");
            return DOM_NO_ELEMENT;
        }

        if ((documentStatus = replaceTextNode(*currentNodeID, textID, dom)) !=
            DOM_SUCCESS) {
            PRINT_ERROR("Failed to replace the text node for a merge.\n");
            return documentStatus;
        }
    } else {
        if ((documentStatus = getNewNodeID(currentNodeID, NODE_TYPE_TEXT,
                                           prevNodeID, dom)) != DOM_SUCCESS) {
            PRINT_ERROR("Failed to create node for domument.\n");
            return documentStatus;
        }

        if (elementToIndex(&dataContainer->text.container,
                           &dataContainer->text.len,
                           &htmlString[elementStartIndex], elementLen, 1, 0,
                           &textID) != ELEMENT_SUCCESS) {
            PRINT_ERROR("Failed to create text ID.\n");
            return DOM_NO_ELEMENT;
        }

        if ((documentStatus = addTextNode(*currentNodeID, textID, dom)) !=
            DOM_SUCCESS) {
            PRINT_ERROR("Failed to add text node to domument.\n");
            return documentStatus;
        }

        if ((documentStatus = setNodeTagID(*currentNodeID, textTagID, dom)) !=
            DOM_SUCCESS) {
            PRINT_ERROR("Failed to set tag ID to text id to domument.\n");
            return documentStatus;
        }
    }

    return documentStatus;
}

DomStatus parse(const char *htmlString, Dom *dom,
                DataContainer *dataContainer) {
    DomStatus documentStatus = DOM_SUCCESS;

    element_id textTagID = 0;
    if (textElementToIndex(&textTagID) != ELEMENT_SUCCESS) {
        PRINT_ERROR("Failed to initialize tag ID for text nodes\n");
        return DOM_NO_ELEMENT;
    }

    size_t currentPosition = 0;

    NodeDepth nodeStack;
    nodeStack.len = 0;

    TextParsing context = BASIC_CONTEXT;

    node_id prevNodeID = 0;
    node_id currentNodeID = 0;
    char ch = htmlString[currentPosition];

    while (ch != '\0') {
        ch = htmlString[currentPosition];
        while (ch == ' ' || isSpecialSpace(ch)) {
            ch = htmlString[++currentPosition];
        }
        if (ch == '\0') {
            break;
        }

        // Text node.
        if (context != BASIC_CONTEXT || ch != '<' ||
            (ch == '<' && (htmlString[currentPosition + 1] == ' ' ||
                           isSpecialSpace(htmlString[currentPosition + 1])))) {
            if (context == BASIC_CONTEXT && ch == '<' &&
                (htmlString[currentPosition + 1] == ' ' ||
                 isSpecialSpace(htmlString[currentPosition + 1]))) {
                context = ROGUE_OPEN_TAG;
            }
            unsigned char isMerge = 0;

            if ((documentStatus = parseTextNode(
                     htmlString, &currentPosition, &prevNodeID, &currentNodeID,
                     textTagID, &context, &isMerge, dom, dataContainer)) !=
                DOM_SUCCESS) {
                return documentStatus;
            }

            if (context == ROGUE_OPEN_TAG) {
                context = BASIC_CONTEXT;
            }

            if (!isMerge) {
                if ((documentStatus =
                         updateReferences(currentNodeID, prevNodeID, &nodeStack,
                                          dom)) != DOM_SUCCESS) {
                    return documentStatus;
                }
            }
        }
        // dom node.
        else {
            // Closing tags.
            if (htmlString[currentPosition + 1] == '/') {
                while (ch != '\0' && ch != '>') {
                    ch = htmlString[++currentPosition];
                }
                if (ch != '\0') {
                    ch = htmlString[++currentPosition];
                }

                if (nodeStack.len > 0) {
                    nodeStack.len--;
                    currentNodeID = nodeStack.stack[nodeStack.len];
                }
                context = BASIC_CONTEXT;
            }
            // Comments or <!domTYPE>.
            else if (htmlString[currentPosition + 1] == '!') {
                // Skip comments.
                if (htmlString[currentPosition + 2] == '-' &&
                    htmlString[currentPosition + 3] == '-') {
                    while (ch != '\0' &&
                           (ch != '>' ||
                            (currentPosition >= 1 &&
                             htmlString[currentPosition - 1] != '-') ||
                            (currentPosition >= 2 &&
                             htmlString[currentPosition - 2] != '-'))) {
                        ch = htmlString[++currentPosition];
                    }
                    if (ch != '\0') {
                        ch = htmlString[++currentPosition];
                    }

                }
                // Any <! is treated as a standard single tag and during
                // printing !domTYPE is special case.
                else {
                    if ((documentStatus = parseExclamdomNode(
                             htmlString, &currentPosition, &prevNodeID,
                             &currentNodeID, dom, dataContainer)) !=
                        DOM_SUCCESS) {
                        return documentStatus;
                    }
                    if ((documentStatus = updateReferences(
                             currentNodeID, prevNodeID, &nodeStack, dom)) !=
                        DOM_SUCCESS) {
                        return documentStatus;
                    }
                }
            }
            // basic dom node.
            else {
                unsigned char isSingle = 0;
                if ((documentStatus = parseBasicdomNode(
                         htmlString, &currentPosition, &prevNodeID,
                         &currentNodeID, &isSingle, &context, dom,
                         dataContainer)) != DOM_SUCCESS) {
                    return documentStatus;
                }
                if ((documentStatus =
                         updateReferences(currentNodeID, prevNodeID, &nodeStack,
                                          dom)) != DOM_SUCCESS) {
                    return documentStatus;
                }
                if (!isSingle) {
                    nodeStack.stack[nodeStack.len] = currentNodeID;
                    nodeStack.len++;
                }
            }
        }
    }

    return documentStatus;
}
