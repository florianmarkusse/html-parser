#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dom/document-utils.h"
#include "dom/document.h"
#include "parse/parse-property.h"
#include "parse/parse.h"
#include "type/element/elements.h"
#include "utils/print/error.h"
#include "utils/text/text.h"

typedef enum {
    BASIC_CONTEXT,
    SCRIPT_CONTEXT,
    STYLE_CONTEXT,
    ROGUE_OPEN_TAG
} TextParsing;

unsigned char isSpecialSpace(char ch) {
    return ch == '\t' || ch == '\n' || ch == '\r';
}

DocumentStatus getNewNodeID(node_id *currentNodeID, node_id *prevNodeID,
                            Document *doc) {
    *prevNodeID = *currentNodeID;
    return createNode(currentNodeID, doc);
}

typedef struct {
    node_id stack[MAX_NODE_DEPTH];
    size_t len;
} __attribute__((aligned(128))) NodeDepth;

DocumentStatus updateReferences(const node_id newNodeID,
                                const node_id previousNodeID,
                                const NodeDepth *depthStack, Document *doc) {
    DocumentStatus documentStatus = DOCUMENT_SUCCESS;

    if (newNodeID > 0 && previousNodeID > 0) {
        if (depthStack->len == 0) {
            if ((documentStatus = addNextNode(previousNodeID, newNodeID,
                                              doc)) != DOCUMENT_SUCCESS) {
                return documentStatus;
            }
        } else {
            const unsigned int parentNodeID =
                depthStack->stack[depthStack->len - 1];
            if ((documentStatus = addParentChild(parentNodeID, newNodeID,
                                                 doc)) != DOCUMENT_SUCCESS) {
                return documentStatus;
            }
            if (parentNodeID == previousNodeID) {
                if ((documentStatus = addParentFirstChild(
                         parentNodeID, newNodeID, doc)) != DOCUMENT_SUCCESS) {
                    return documentStatus;
                }
            } else {
                if ((documentStatus = addNextNode(previousNodeID, newNodeID,
                                                  doc)) != DOCUMENT_SUCCESS) {
                    return documentStatus;
                }
            }
        }
    }

    return documentStatus;
}

DocumentStatus parseDocNode(const char *htmlString, size_t *currentPosition,
                            node_id *prevNodeID, node_id *newNodeID,
                            unsigned char *isSingle, TextParsing *context,
                            unsigned char exclamStart, Document *doc) {
    DocumentStatus documentStatus = DOCUMENT_SUCCESS;
    char ch = htmlString[++(*currentPosition)];

    if ((documentStatus = getNewNodeID(newNodeID, prevNodeID, doc)) !=
        DOCUMENT_SUCCESS) {
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
        if (ch == '=') {
            element_id attrValueID = 0;

            // Expected syntax: key="value" OR
            // Expected syntax: key='value'
            // We can do some more interesting stuff but
            // currently not required.
            if (elementToIndex(&gPropKeys.container, &gPropKeys.pairedLen,
                               &htmlString[attrKeyStartIndex], attrKeyLen, 1, 1,
                               &attrKeyID) != ELEMENT_SUCCESS) {
                PRINT_ERROR("Failed to create element ID for key.\n");
                return DOCUMENT_NO_ELEMENT;
            }
            ch = htmlString[++(*currentPosition)];
            char quote = ch;
            ch = htmlString[++(*currentPosition)];

            size_t attrValueStartIndex = *currentPosition;
            while (ch != quote) {
                ch = htmlString[++(*currentPosition)];
            }
            size_t attrValueLen = *currentPosition - attrValueStartIndex;

            if (elementToIndex(&gPropValues.container, &gPropValues.len,
                               &htmlString[attrValueStartIndex], attrValueLen,
                               1, 1, &attrValueID) != ELEMENT_SUCCESS) {
                PRINT_ERROR("Failed to create element ID for value.\n");
                return DOCUMENT_NO_ELEMENT;
            }

            if ((documentStatus =
                     addProperty(*newNodeID, attrKeyID, attrValueID, doc)) !=
                DOCUMENT_SUCCESS) {
                PRINT_ERROR("Failed to add key-value property.\n");
                return DOCUMENT_NO_ELEMENT;
            }

            // Move past '"'
            ch = htmlString[++(*currentPosition)];
        } else {
            if (elementToIndex(&gPropKeys.container, &gPropKeys.singleLen,
                               &htmlString[attrKeyStartIndex], attrKeyLen, 0, 1,
                               &attrKeyID) != ELEMENT_SUCCESS) {
                PRINT_ERROR("Failed to create element ID for key.\n");
                return DOCUMENT_NO_ELEMENT;
            }
            if ((documentStatus = addBooleanProperty(
                     *newNodeID, attrKeyID, doc)) != DOCUMENT_SUCCESS) {
                PRINT_ERROR("Failed to add boolean property.\n");
                return documentStatus;
            }
        }
    }
    if (ch == '/') {
        *isSingle = 1;
    }

    element_id tagID = 0;
    element_id *elementTypeLen = &gTags.pairedLen;
    if (*isSingle) {
        elementTypeLen = &gTags.singleLen;
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

    if (elementToIndex(&gTags.container, elementTypeLen,
                       &htmlString[elementStartIndex], elementLen, !(*isSingle),
                       1, &tagID) != ELEMENT_SUCCESS) {
        PRINT_ERROR("Failed to create tag ID for element "
                    "starting with '!'.\n");
        return DOCUMENT_NO_ELEMENT;
    }

    if ((documentStatus = setTagID(*newNodeID, tagID, doc)) !=
        DOCUMENT_SUCCESS) {
        PRINT_ERROR("Failed to set tag ID to text id to document.\n");
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

DocumentStatus parseBasicDocNode(const char *htmlString,
                                 size_t *currentPosition, node_id *prevNodeID,
                                 node_id *newNodeID, unsigned char *isSingle,
                                 TextParsing *context, Document *doc) {
    return parseDocNode(htmlString, currentPosition, prevNodeID, newNodeID,
                        isSingle, context, 0, doc);
}

DocumentStatus parseExclamDocNode(const char *htmlString,
                                  size_t *currentPosition, node_id *prevNodeID,
                                  node_id *newNodeID, Document *doc) {
    unsigned char ignore = 0;
    TextParsing ignore2 = BASIC_CONTEXT;
    return parseDocNode(htmlString, currentPosition, prevNodeID, newNodeID,
                        &ignore, &ignore2, 1, doc);
}

unsigned char textNodeAtBasicEnd(const char ch, const char *htmlString,
                                 const size_t currentPosition) {
    return (ch != '\0' && !isSpecialSpace(ch) &&
            (ch != ' ' ||
             (currentPosition > 0 && htmlString[currentPosition - 1] != ' ')));
}

DocumentStatus parseTextNode(const char *htmlString, size_t *currentPosition,
                             node_id *prevNodeID, node_id *currentNodeID,
                             const element_id textTagID, TextParsing *context,
                             unsigned char *isMerge, Document *doc) {
    DocumentStatus documentStatus = DOCUMENT_SUCCESS;
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

    Node prevNode = doc->nodes[*currentNodeID];
    if (isText(prevNode.tagID)) {
        *isMerge = 1;
        const char *prevText = getText(prevNode.nodeID, doc);
        const size_t mergedLen = strlen(prevText) + elementLen +
                                 2; // Adding a whitespace in between.

        char buffer[mergedLen];
        strcpy(buffer, prevText);
        strcat(buffer, " ");
        strncat(buffer, &htmlString[elementStartIndex], elementLen);
        buffer[mergedLen - 1] = '\0';

        if (elementToIndex(&gText.container, &gText.len, buffer, mergedLen, 1,
                           0, &textID) != ELEMENT_SUCCESS) {
            PRINT_ERROR("Failed to create text ID for merging text nodes.\n");
            return DOCUMENT_NO_ELEMENT;
        }

        if ((documentStatus = replaceTextNode(*currentNodeID, textID, doc)) !=
            DOCUMENT_SUCCESS) {
            PRINT_ERROR("Failed to replace the text node for a merge.\n");
            return documentStatus;
        }
    } else {
        if ((documentStatus = getNewNodeID(currentNodeID, prevNodeID, doc)) !=
            DOCUMENT_SUCCESS) {
            PRINT_ERROR("Failed to create node for document.\n");
            return documentStatus;
        }

        if (elementToIndex(&gText.container, &gText.len,
                           &htmlString[elementStartIndex], elementLen, 1, 0,
                           &textID) != ELEMENT_SUCCESS) {
            PRINT_ERROR("Failed to create text ID.\n");
            return DOCUMENT_NO_ELEMENT;
        }

        if ((documentStatus = addTextNode(*currentNodeID, textID, doc)) !=
            DOCUMENT_SUCCESS) {
            PRINT_ERROR("Failed to add text node to document.\n");
            return documentStatus;
        }

        if ((documentStatus = setTagID(*currentNodeID, textTagID, doc)) !=
            DOCUMENT_SUCCESS) {
            PRINT_ERROR("Failed to set tag ID to text id to document.\n");
            return documentStatus;
        }
    }

    return documentStatus;
}

DocumentStatus parse(const char *htmlString, Document *doc) {
    DocumentStatus documentStatus = DOCUMENT_SUCCESS;

    element_id textTagID = 0;
    if (textElementToIndex(&textTagID) != ELEMENT_SUCCESS) {
        PRINT_ERROR("Failed to initialize tag ID for text nodes\n");
        return DOCUMENT_NO_ELEMENT;
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
                     textTagID, &context, &isMerge, doc)) != DOCUMENT_SUCCESS) {
                return documentStatus;
            }

            if (context == ROGUE_OPEN_TAG) {
                context = BASIC_CONTEXT;
            }

            if (!isMerge) {
                if ((documentStatus =
                         updateReferences(currentNodeID, prevNodeID, &nodeStack,
                                          doc)) != DOCUMENT_SUCCESS) {
                    return documentStatus;
                }
            }
        }
        // Doc node.
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
                }
                currentNodeID = nodeStack.stack[nodeStack.len];
                context = BASIC_CONTEXT;
            }
            // Comments or <!DOCTYPE>.
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
                // printing !DOCTYPE is special case.
                else {
                    if ((documentStatus = parseExclamDocNode(
                             htmlString, &currentPosition, &prevNodeID,
                             &currentNodeID, doc)) != DOCUMENT_SUCCESS) {
                        return documentStatus;
                    }
                    if ((documentStatus = updateReferences(
                             currentNodeID, prevNodeID, &nodeStack, doc)) !=
                        DOCUMENT_SUCCESS) {
                        return documentStatus;
                    }
                }
            }
            // basic doc node.
            else {
                unsigned char isSingle = 0;
                if ((documentStatus = parseBasicDocNode(
                         htmlString, &currentPosition, &prevNodeID,
                         &currentNodeID, &isSingle, &context, doc)) !=
                    DOCUMENT_SUCCESS) {
                    return documentStatus;
                }
                if ((documentStatus =
                         updateReferences(currentNodeID, prevNodeID, &nodeStack,
                                          doc)) != DOCUMENT_SUCCESS) {
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
