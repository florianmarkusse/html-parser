#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/dom/appendix/appendix.h"
#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/data/definitions.h"
#include "flo/html-parser/type/element/elements-print.h"
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

const char *const voidElementTags[] = {
    "area",  "base", "br",   "col",    "embed", "hr", "img",
    "input", "link", "meta", "source", "track", "wbr"};

bool isVoidElement(const char *str, size_t len) {
    for (size_t i = 0; i < sizeof(voidElementTags) / sizeof(voidElementTags[0]);
         i++) {
        if (strncmp(str, voidElementTags[i], len) == 0 &&
            strlen(voidElementTags[i]) == len) {
            return true; // Match found
        }
    }
    return false; // No match found
}

flo_html_DomStatus getNewNodeID(flo_html_node_id *lastParsedNodeID,
                                const flo_html_NodeType nodeType,
                                flo_html_node_id *prevNodeID,
                                flo_html_Dom *dom) {
    *prevNodeID = *lastParsedNodeID;
    return flo_html_createNode(lastParsedNodeID, nodeType, dom);
}

static flo_html_DomStatus
updateReferences(const flo_html_node_id newNodeID,
                 const flo_html_node_id previousNodeID,
                 const flo_html_NodeDepth *depthStack, flo_html_Dom *dom) {
    flo_html_DomStatus documentStatus = DOM_SUCCESS;

    if (newNodeID > 0 && previousNodeID > 0) {
        if (depthStack->len == 0) {
            if ((documentStatus = flo_html_addNextNode(
                     previousNodeID, newNodeID, dom)) != DOM_SUCCESS) {
                return documentStatus;
            }
        } else {
            const unsigned int parentNodeID =
                depthStack->stack[depthStack->len - 1];
            if ((documentStatus = flo_html_addParentChild(
                     parentNodeID, newNodeID, dom)) != DOM_SUCCESS) {
                return documentStatus;
            }
            if (parentNodeID == previousNodeID) {
                if ((documentStatus = flo_html_addParentFirstChild(
                         parentNodeID, newNodeID, dom)) != DOM_SUCCESS) {
                    return documentStatus;
                }
            } else {
                if ((documentStatus = flo_html_addNextNode(
                         previousNodeID, newNodeID, dom)) != DOM_SUCCESS) {
                    return documentStatus;
                }
            }
        }
    }

    return documentStatus;
}

flo_html_DomStatus
parseflo_html_DomNode(const char *htmlString, size_t *currentPosition,
                      flo_html_node_id *prevNodeID, flo_html_node_id *newNodeID,
                      unsigned char *isSingle, TextParsing *context,
                      unsigned char exclamStart, flo_html_Dom *dom,
                      flo_html_TextStore *textStore) {
    flo_html_ElementStatus elementStatus = ELEMENT_SUCCESS;
    flo_html_DomStatus documentStatus = DOM_SUCCESS;
    char ch = htmlString[++(*currentPosition)];

    if ((documentStatus = getNewNodeID(newNodeID, NODE_TYPE_DOCUMENT,
                                       prevNodeID, dom)) != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to create node.\n");
        return documentStatus;
    }

    size_t elementStartIndex = *currentPosition;
    while (ch != ' ' && !flo_html_isSpecialSpace(ch) && ch != '>' &&
           ch != '\0') {
        ch = htmlString[++(*currentPosition)];
    }
    size_t elementLen = *currentPosition - elementStartIndex;
    if (ch == '\0') {
        elementLen--;
    }

    *isSingle = exclamStart;
    if (ch == '>' && *currentPosition > 0 &&
        htmlString[*currentPosition - 1] == '/') {
        *isSingle = true;
        elementLen--;
    } else if (isVoidElement(&htmlString[elementStartIndex], elementLen)) {
        // For the funny <br> tags...
        *isSingle = true;
    }

    // Collect attributes here.
    while (ch != '>' && ch != '/' && ch != '\0') {
        while (ch == ' ' || flo_html_isSpecialSpace(ch)) {
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

        if (ch == '=') {
            // Expected syntax: key="value" OR
            // Expected syntax: key='value' OR
            // Expected syntax: key=value (This is invalid html, but will still
            // support it) We can do some more interesting stuff but currently
            // not required.
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
                while (ch != ' ' && !flo_html_isSpecialSpace(ch) && ch != '>' &&
                       ch != '\0') {
                    ch = htmlString[++(*currentPosition)];
                }
            }

            size_t attrValueLen = *currentPosition - attrValueStartIndex;

            elementStatus = flo_html_addPropertyToNodeStringsWithLength(
                *newNodeID, &htmlString[attrKeyStartIndex], attrKeyLen,
                &htmlString[attrValueStartIndex], attrValueLen, dom, textStore);
            if (elementStatus != ELEMENT_SUCCESS) {
                FLO_HTML_ERROR_WITH_CODE_ONLY(
                    flo_html_elementStatusToString(elementStatus),
                    "Failed to add key-value property!\n");
                return DOM_NO_ELEMENT;
            }

            // Move past '"'
            ch = htmlString[++(*currentPosition)];
        } else {
            elementStatus = flo_html_addBooleanPropertyToNodeStringWithLength(
                *newNodeID, &htmlString[attrKeyStartIndex], attrKeyLen, dom,
                textStore);

            if (elementStatus != ELEMENT_SUCCESS) {
                FLO_HTML_ERROR_WITH_CODE_ONLY(
                    flo_html_elementStatusToString(elementStatus),
                    "Failed to add boolean property!\n");
                return DOM_NO_ELEMENT;
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

    if ((documentStatus = flo_html_setTagOnDocumentNode(
             &htmlString[elementStartIndex], elementLen, *newNodeID,
             !(*isSingle), dom, textStore)) != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add tag to node ID.\n");
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

flo_html_DomStatus
parseBasicdomNode(const char *htmlString, size_t *currentPosition,
                  flo_html_node_id *prevNodeID, flo_html_node_id *newNodeID,
                  unsigned char *isSingle, TextParsing *context,
                  flo_html_Dom *dom, flo_html_TextStore *textStore) {
    return parseflo_html_DomNode(htmlString, currentPosition, prevNodeID,
                                 newNodeID, isSingle, context, 0, dom,
                                 textStore);
}

flo_html_DomStatus
parseExclamdomNode(const char *htmlString, size_t *currentPosition,
                   flo_html_node_id *prevNodeID, flo_html_node_id *newNodeID,
                   flo_html_Dom *dom, flo_html_TextStore *textStore) {
    unsigned char ignore = 0;
    TextParsing ignore2 = BASIC_CONTEXT;
    return parseflo_html_DomNode(htmlString, currentPosition, prevNodeID,
                                 newNodeID, &ignore, &ignore2, 1, dom,
                                 textStore);
}

unsigned char textNodeAtBasicEnd(const char ch, const char *htmlString,
                                 const size_t currentPosition) {
    return (ch != '\0' && !flo_html_isSpecialSpace(ch) &&
            (ch != ' ' ||
             (currentPosition > 0 && htmlString[currentPosition - 1] != ' ')));
}

flo_html_DomStatus
parseTextNode(const char *htmlString, size_t *currentPosition,
              flo_html_node_id *prevNodeID, flo_html_node_id *lastParsedNodeID,
              TextParsing *context, unsigned char *isMerge, flo_html_Dom *dom,
              flo_html_TextStore *textStore) {
    flo_html_ElementStatus elementStatus = ELEMENT_SUCCESS;
    flo_html_DomStatus documentStatus = DOM_SUCCESS;
    size_t elementStartIndex = *currentPosition;
    char ch = htmlString[*currentPosition];
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

    flo_html_Node *prevNode = &dom->nodes[*lastParsedNodeID];
    if (prevNode->nodeType == NODE_TYPE_TEXT) {
        *isMerge = 1;
        elementStatus =
            flo_html_addTextToTextNode(prevNode, &htmlString[elementStartIndex],
                                       elementLen, dom, textStore, true);
        if (elementStatus != ELEMENT_CREATED) {
            FLO_HTML_ERROR_WITH_CODE_ONLY(
                flo_html_elementStatusToString(elementStatus),
                "Failed to insert text");
            return DOM_NO_ELEMENT;
        }
    } else {
        if ((documentStatus = getNewNodeID(lastParsedNodeID, NODE_TYPE_TEXT,
                                           prevNodeID, dom)) != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to create node for domument.\n");
            return documentStatus;
        }

        char *dataLocation = NULL;
        elementStatus = flo_html_insertElement(&textStore->text,
                                               &htmlString[elementStartIndex],
                                               elementLen, &dataLocation);
        if (elementStatus != ELEMENT_CREATED) {
            FLO_HTML_ERROR_WITH_CODE_ONLY(
                flo_html_elementStatusToString(elementStatus),
                "Failed to index text!\n");
            return DOM_NO_ELEMENT;
        }

        flo_html_setNodeText(*lastParsedNodeID, dataLocation, dom);
    }

    return documentStatus;
}

flo_html_DomStatus flo_html_parse(const char *htmlString, flo_html_Dom *dom,
                                  flo_html_TextStore *textStore) {
    flo_html_DomStatus documentStatus = DOM_SUCCESS;

    size_t currentPosition = 0;

    flo_html_NodeDepth nodeStack;
    nodeStack.len = 0;

    TextParsing context = BASIC_CONTEXT;

    flo_html_node_id prevNodeID = 0;
    flo_html_node_id lastParsedNodeID = 0;
    char ch = htmlString[currentPosition];

    while (ch != '\0') {
        ch = htmlString[currentPosition];
        while (ch == ' ' || flo_html_isSpecialSpace(ch)) {
            ch = htmlString[++currentPosition];
        }
        if (ch == '\0') {
            break;
        }

        // Text node.
        if (context != BASIC_CONTEXT || ch != '<' ||
            (ch == '<' &&
             (htmlString[currentPosition + 1] == ' ' ||
              flo_html_isSpecialSpace(htmlString[currentPosition + 1])))) {
            if (context == BASIC_CONTEXT && ch == '<' &&
                (htmlString[currentPosition + 1] == ' ' ||
                 flo_html_isSpecialSpace(htmlString[currentPosition + 1]))) {
                context = ROGUE_OPEN_TAG;
            }
            unsigned char isMerge = 0;

            if ((documentStatus =
                     parseTextNode(htmlString, &currentPosition, &prevNodeID,
                                   &lastParsedNodeID, &context, &isMerge, dom,
                                   textStore)) != DOM_SUCCESS) {
                return documentStatus;
            }

            if (context == ROGUE_OPEN_TAG) {
                context = BASIC_CONTEXT;
            }

            if (!isMerge) {
                if ((documentStatus =
                         updateReferences(lastParsedNodeID, prevNodeID,
                                          &nodeStack, dom)) != DOM_SUCCESS) {
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
                    lastParsedNodeID = nodeStack.stack[nodeStack.len];
                }
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
                    if ((documentStatus = parseExclamdomNode(
                             htmlString, &currentPosition, &prevNodeID,
                             &lastParsedNodeID, dom, textStore)) !=
                        DOM_SUCCESS) {
                        return documentStatus;
                    }
                    if ((documentStatus = updateReferences(
                             lastParsedNodeID, prevNodeID, &nodeStack, dom)) !=
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
                         &lastParsedNodeID, &isSingle, &context, dom,
                         textStore)) != DOM_SUCCESS) {
                    return documentStatus;
                }
                if ((documentStatus =
                         updateReferences(lastParsedNodeID, prevNodeID,
                                          &nodeStack, dom)) != DOM_SUCCESS) {
                    return documentStatus;
                }
                if (!isSingle) {
                    if (nodeStack.len >= FLO_HTML_MAX_NODE_DEPTH) {
                        FLO_HTML_PRINT_ERROR(
                            "Reached max node depth, aborting\n");
                        return DOM_TOO_DEEP;
                    }
                    nodeStack.stack[nodeStack.len] = lastParsedNodeID;
                    nodeStack.len++;
                }
            }
        }
    }

    return documentStatus;
}

flo_html_DomStatus
flo_html_parseDocumentElement(const flo_html_DocumentNode *documentNode,
                              flo_html_Dom *dom, flo_html_TextStore *textStore,
                              flo_html_node_id *newNodeID) {
    flo_html_DomStatus domStatus =
        flo_html_createNode(newNodeID, NODE_TYPE_DOCUMENT, dom);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to retrieve a new node ID!\n");
        return domStatus;
    }

    domStatus = flo_html_setTagOnDocumentNode(
        documentNode->tag, strlen(documentNode->tag), *newNodeID,
        documentNode->isPaired, dom, textStore);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add tag to new node ID!\n");
        return domStatus;
    }

    for (size_t i = 0; i < documentNode->boolPropsLen; i++) {
        const char *boolProp = documentNode->boolProps[i];
        flo_html_ElementStatus elementStatus =
            flo_html_addBooleanPropertyToNodeString(*newNodeID, boolProp, dom,
                                                    textStore);
        if (elementStatus != ELEMENT_SUCCESS) {
            FLO_HTML_PRINT_ERROR(
                "Failed to boolean property to new node ID!\n");
            return DOM_NO_ELEMENT;
        }
    }

    for (size_t i = 0; i < documentNode->propsLen; i++) {
        const char *keyProp = documentNode->keyProps[i];
        const char *valueProp = documentNode->valueProps[i];
        flo_html_ElementStatus elementStatus =
            flo_html_addPropertyToNodeStrings(*newNodeID, keyProp, valueProp,
                                              dom, textStore);
        if (elementStatus != ELEMENT_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to property to new node ID!\n");
            return DOM_NO_ELEMENT;
        }
    }

    return domStatus;
}

flo_html_DomStatus flo_html_parseTextElement(const flo_html_String text,
                                             flo_html_Dom *dom,
                                             flo_html_TextStore *textStore,
                                             flo_html_node_id *newNodeID) {
    flo_html_DomStatus domStatus =
        flo_html_createNode(newNodeID, NODE_TYPE_TEXT, dom);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to retrieve a new node ID!\n");
        return domStatus;
    }

    char *dataLocation = NULL;
    flo_html_ElementStatus elementStatus = flo_html_insertElement(
        &textStore->text, text, strlen(text), &dataLocation);
    if (elementStatus != ELEMENT_CREATED) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(elementStatus),
            "Failed to index text!\n");
        return DOM_NO_ELEMENT;
    }

    flo_html_setNodeText(*newNodeID, dataLocation, dom);

    return domStatus;
}
