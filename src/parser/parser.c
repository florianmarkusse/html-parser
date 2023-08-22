#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/utils/print/error.h"
#include "flo/html-parser/utils/text/text.h"

typedef enum {
    BASIC_CONTEXT,
    SCRIPT_CONTEXT,
    STYLE_CONTEXT,
    ROGUE_OPEN_TAG
} TextParsing;

DomStatus getNewNodeID(node_id *currentNodeID, node_id *prevNodeID, Dom *dom) {
    *prevNodeID = *currentNodeID;
    return createNode(currentNodeID, dom);
}

typedef struct {
    node_id stack[MAX_NODE_DEPTH];
    size_t len;
} __attribute__((aligned(128))) NodeDepth;

DomStatus updateReferences(const node_id newNodeID,
                           const node_id previousNodeID,
                           const NodeDepth *depthStack, Dom *dom) {
    DomStatus domumentStatus = DOM_SUCCESS;

    if (newNodeID > 0 && previousNodeID > 0) {
        if (depthStack->len == 0) {
            if ((domumentStatus = addNextNode(previousNodeID, newNodeID,
                                              dom)) != DOM_SUCCESS) {
                return domumentStatus;
            }
        } else {
            const unsigned int parentNodeID =
                depthStack->stack[depthStack->len - 1];
            if ((domumentStatus = addParentChild(parentNodeID, newNodeID,
                                                 dom)) != DOM_SUCCESS) {
                return domumentStatus;
            }
            if (parentNodeID == previousNodeID) {
                if ((domumentStatus = addParentFirstChild(
                         parentNodeID, newNodeID, dom)) != DOM_SUCCESS) {
                    return domumentStatus;
                }
            } else {
                if ((domumentStatus = addNextNode(previousNodeID, newNodeID,
                                                  dom)) != DOM_SUCCESS) {
                    return domumentStatus;
                }
            }
        }
    }

    return domumentStatus;
}

DomStatus parsedomNode(const char *htmlString, size_t *currentPosition,
                       node_id *prevNodeID, node_id *newNodeID,
                       unsigned char *isSingle, TextParsing *context,
                       unsigned char exclamStart, Dom *dom,
                       DataContainer *dataContainer) {
    DomStatus domumentStatus = DOM_SUCCESS;
    char ch = htmlString[++(*currentPosition)];

    if ((domumentStatus = getNewNodeID(newNodeID, prevNodeID, dom)) !=
        DOM_SUCCESS) {
        PRINT_ERROR("Failed to create node.\n");
        return domumentStatus;
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
        if (ch == '=') {
            element_id attrValueID = 0;

            // Expected syntax: key="value" OR
            // Expected syntax: key='value' OR
            // Expected syntax: key=value (This is invalid html, but will still
            // support it) We can do some more interesting stuff but currently
            // not required.
            if (elementToIndex(&dataContainer->propKeys.container,
                               &dataContainer->propKeys.pairedLen,
                               &htmlString[attrKeyStartIndex], attrKeyLen, 1, 1,
                               &attrKeyID) != ELEMENT_SUCCESS) {
                PRINT_ERROR("Failed to create element ID for key.\n");
                return DOM_NO_ELEMENT;
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

            if (elementToIndex(&dataContainer->propValues.container,
                               &dataContainer->propValues.len,
                               &htmlString[attrValueStartIndex], attrValueLen,
                               1, 1, &attrValueID) != ELEMENT_SUCCESS) {
                PRINT_ERROR("Failed to create element ID for value.\n");
                return DOM_NO_ELEMENT;
            }

            if ((domumentStatus = addProperty(
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
            if ((domumentStatus = addBooleanProperty(*newNodeID, attrKeyID,
                                                     dom)) != DOM_SUCCESS) {
                PRINT_ERROR("Failed to add boolean property.\n");
                return domumentStatus;
            }
        }
    }
    if (ch == '/') {
        *isSingle = 1;
    }

    element_id tagID = 0;
    element_id *elementTypeLen = &dataContainer->tags.pairedLen;
    if (*isSingle) {
        elementTypeLen = &dataContainer->tags.singleLen;
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

    if (newElementToIndex(&dataContainer->tagNames,
                          &htmlString[elementStartIndex], elementLen,
                          !(*isSingle), 1) != ELEMENT_SUCCESS) {
        PRINT_ERROR("Failed to insert into new tag names!\n");
        return DOM_NO_ELEMENT;
    }

    if (elementToIndex(&dataContainer->tags.container, elementTypeLen,
                       &htmlString[elementStartIndex], elementLen, !(*isSingle),
                       1, &tagID) != ELEMENT_SUCCESS) {
        PRINT_ERROR("Failed to create tag ID for element!\n");
        return DOM_NO_ELEMENT;
    }

    if ((domumentStatus = setTagID(*newNodeID, tagID, dom)) != DOM_SUCCESS) {
        PRINT_ERROR("Failed to set tag ID to text id to domument.\n");
        return domumentStatus;
    }

    while (ch != '>' && ch != '\0') {
        ch = htmlString[++(*currentPosition)];
    }
    if (ch != '\0') {
        ch = htmlString[++(*currentPosition)];
    }

    return domumentStatus;
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
    DomStatus domumentStatus = DOM_SUCCESS;
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
                return domumentStatus;
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
                return domumentStatus;
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
    if (isText(prevNode.tagID)) {
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

        if ((domumentStatus = replaceTextNode(*currentNodeID, textID, dom)) !=
            DOM_SUCCESS) {
            PRINT_ERROR("Failed to replace the text node for a merge.\n");
            return domumentStatus;
        }
    } else {
        if ((domumentStatus = getNewNodeID(currentNodeID, prevNodeID, dom)) !=
            DOM_SUCCESS) {
            PRINT_ERROR("Failed to create node for domument.\n");
            return domumentStatus;
        }

        if (elementToIndex(&dataContainer->text.container,
                           &dataContainer->text.len,
                           &htmlString[elementStartIndex], elementLen, 1, 0,
                           &textID) != ELEMENT_SUCCESS) {
            PRINT_ERROR("Failed to create text ID.\n");
            return DOM_NO_ELEMENT;
        }

        if ((domumentStatus = addTextNode(*currentNodeID, textID, dom)) !=
            DOM_SUCCESS) {
            PRINT_ERROR("Failed to add text node to domument.\n");
            return domumentStatus;
        }

        if ((domumentStatus = setTagID(*currentNodeID, textTagID, dom)) !=
            DOM_SUCCESS) {
            PRINT_ERROR("Failed to set tag ID to text id to domument.\n");
            return domumentStatus;
        }
    }

    return domumentStatus;
}

DomStatus parse(const char *htmlString, Dom *dom,
                DataContainer *dataContainer) {
    DomStatus domumentStatus = DOM_SUCCESS;

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

            if ((domumentStatus = parseTextNode(
                     htmlString, &currentPosition, &prevNodeID, &currentNodeID,
                     textTagID, &context, &isMerge, dom, dataContainer)) !=
                DOM_SUCCESS) {
                return domumentStatus;
            }

            if (context == ROGUE_OPEN_TAG) {
                context = BASIC_CONTEXT;
            }

            if (!isMerge) {
                if ((domumentStatus =
                         updateReferences(currentNodeID, prevNodeID, &nodeStack,
                                          dom)) != DOM_SUCCESS) {
                    return domumentStatus;
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
                    if ((domumentStatus = parseExclamdomNode(
                             htmlString, &currentPosition, &prevNodeID,
                             &currentNodeID, dom, dataContainer)) !=
                        DOM_SUCCESS) {
                        return domumentStatus;
                    }
                    if ((domumentStatus = updateReferences(
                             currentNodeID, prevNodeID, &nodeStack, dom)) !=
                        DOM_SUCCESS) {
                        return domumentStatus;
                    }
                }
            }
            // basic dom node.
            else {
                unsigned char isSingle = 0;
                if ((domumentStatus = parseBasicdomNode(
                         htmlString, &currentPosition, &prevNodeID,
                         &currentNodeID, &isSingle, &context, dom,
                         dataContainer)) != DOM_SUCCESS) {
                    return domumentStatus;
                }
                if ((domumentStatus =
                         updateReferences(currentNodeID, prevNodeID, &nodeStack,
                                          dom)) != DOM_SUCCESS) {
                    return domumentStatus;
                }
                if (!isSingle) {
                    nodeStack.stack[nodeStack.len] = currentNodeID;
                    nodeStack.len++;
                }
            }
        }
    }

    return domumentStatus;
}
