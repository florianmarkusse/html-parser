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

const flo_html_String voidElementTags[] = {
    FLO_HTML_S("area"), FLO_HTML_S("base"),   FLO_HTML_S("br"),
    FLO_HTML_S("col"),  FLO_HTML_S("embed"),  FLO_HTML_S("hr"),
    FLO_HTML_S("img"),  FLO_HTML_S("input"),  FLO_HTML_S("link"),
    FLO_HTML_S("meta"), FLO_HTML_S("source"), FLO_HTML_S("track"),
    FLO_HTML_S("wbr")};

bool isVoidElement(const flo_html_String str) {
    for (size_t i = 0; i < sizeof(voidElementTags) / sizeof(voidElementTags[0]);
         i++) {
        if (flo_html_stringEquals(str, voidElementTags[i])) {
            return true;
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
parseflo_html_DomNode(const flo_html_String htmlString, size_t *currentPosition,
                      flo_html_node_id *prevNodeID, flo_html_node_id *newNodeID,
                      bool *isSingle, TextParsing *context,
                      unsigned char exclamStart, flo_html_Dom *dom,
                      flo_html_TextStore *textStore) {
    flo_html_ElementStatus elementStatus = ELEMENT_SUCCESS;
    flo_html_DomStatus documentStatus = DOM_SUCCESS;
    unsigned char ch = flo_html_getChar(htmlString, ++(*currentPosition));

    if ((documentStatus = getNewNodeID(newNodeID, NODE_TYPE_DOCUMENT,
                                       prevNodeID, dom)) != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to create node.\n");
        return documentStatus;
    }

    size_t elementStartIndex = *currentPosition;
    while (ch != ' ' && !flo_html_isSpecialSpace(ch) && ch != '>' &&
           ch != '\0') {
        ch = flo_html_getChar(htmlString, ++(*currentPosition));
    }
    size_t elementLen = *currentPosition - elementStartIndex;
    if (ch == '\0') {
        elementLen--;
    }

    *isSingle = exclamStart;
    if (ch == '>' && *currentPosition > 0 &&
        htmlString.buf[*currentPosition - 1] == '/') {
        *isSingle = true;
        elementLen--;
    } else if (isVoidElement(FLO_HTML_S_LEN(&htmlString.buf[elementStartIndex],
                                            elementLen))) {
        // For the funny <br> tags...
        *isSingle = true;
    }

    // Collect attributes here.
    while (ch != '>' && ch != '/' && ch != '\0') {
        while (ch == ' ' || flo_html_isSpecialSpace(ch)) {
            ch = flo_html_getChar(htmlString, ++(*currentPosition));
        }
        if (ch == '/' || ch == '>') {
            break;
        }

        size_t attrKeyStartIndex = *currentPosition;

        if (ch == '\'' || ch == '"') {
            unsigned char quote = ch;
            ch = flo_html_getChar(htmlString,
                                  ++(*currentPosition)); // Skip start quote.
            while (ch != quote && ch != '\0') {
                ch = flo_html_getChar(htmlString, ++(*currentPosition));
            }
            ch = htmlString.buf[++(*currentPosition)]; // Skip end quote.
        } else {
            while (ch != ' ' && ch != '>' && ch != '=') {
                ch = flo_html_getChar(htmlString, ++(*currentPosition));
            }
        }
        size_t attrKeyLen = *currentPosition - attrKeyStartIndex;
        if (ch == '>' && *currentPosition > 0 &&
            htmlString.buf[*currentPosition - 1] == '/') {
            *isSingle = 1;
            attrKeyLen--;
        }

        if (ch == '=') {
            // Expected syntax: key="value" OR
            // Expected syntax: key='value' OR
            // Expected syntax: key=value (This is invalid html, but will still
            // support it) We can do some more interesting stuff but currently
            // not required.
            ch = flo_html_getChar(htmlString, ++(*currentPosition));

            size_t attrValueStartIndex = *currentPosition;
            if (ch == '\'' || ch == '"') {
                attrValueStartIndex++;
                unsigned char quote = ch;
                ch = flo_html_getChar(htmlString, ++(*currentPosition));

                while (ch != quote && ch != '\0') {
                    ch = flo_html_getChar(htmlString, ++(*currentPosition));
                }
            } else {
                while (ch != ' ' && !flo_html_isSpecialSpace(ch) && ch != '>' &&
                       ch != '\0') {
                    ch = flo_html_getChar(htmlString, ++(*currentPosition));
                }
            }

            size_t attrValueLen = *currentPosition - attrValueStartIndex;

            elementStatus = flo_html_addPropertyToNode(
                *newNodeID,
                FLO_HTML_S_LEN(
                    flo_html_getCharPtr(htmlString, attrKeyStartIndex),
                    attrKeyLen),
                FLO_HTML_S_LEN(
                    flo_html_getCharPtr(htmlString, attrValueStartIndex),
                    attrValueLen),
                dom, textStore);
            if (elementStatus != ELEMENT_SUCCESS) {
                FLO_HTML_ERROR_WITH_CODE_ONLY(
                    flo_html_elementStatusToString(elementStatus),
                    "Failed to add key-value property!\n");
                return DOM_NO_ELEMENT;
            }

            // Move past '"'
            ch = flo_html_getChar(htmlString, ++(*currentPosition));
        } else {
            elementStatus = flo_html_addBooleanPropertyToNode(
                *newNodeID,
                FLO_HTML_S_LEN(
                    flo_html_getCharPtr(htmlString, attrKeyStartIndex),
                    attrKeyLen),
                dom, textStore);

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

    unsigned char tagBuffer[elementLen + 1];
    flo_html_String tagName = {elementLen, tagBuffer};
    tagName = flo_html_strcpy(
        tagName,
        FLO_HTML_S_LEN(flo_html_getCharPtr(htmlString, elementStartIndex),
                       elementLen));

    *context = BASIC_CONTEXT;
    if (flo_html_stringEquals(tagName, FLO_HTML_S("script"))) {
        *context = SCRIPT_CONTEXT;
    }

    if (flo_html_stringEquals(tagName, FLO_HTML_S("style"))) {
        *context = STYLE_CONTEXT;
    }

    if ((documentStatus = flo_html_setTagOnDocumentNode(
             FLO_HTML_S_LEN(flo_html_getCharPtr(htmlString, elementStartIndex),
                            elementLen),
             *newNodeID, !(*isSingle), dom, textStore)) != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add tag to node ID.\n");
        return documentStatus;
    }

    while (ch != '>' && ch != '\0') {
        ch = flo_html_getChar(htmlString, ++(*currentPosition));
    }
    if (ch != '\0') {
        ch = flo_html_getChar(htmlString, ++(*currentPosition));
    }

    return documentStatus;
}

flo_html_DomStatus
parseBasicdomNode(const flo_html_String htmlString, size_t *currentPosition,
                  flo_html_node_id *prevNodeID, flo_html_node_id *newNodeID,
                  bool *isSingle, TextParsing *context, flo_html_Dom *dom,
                  flo_html_TextStore *textStore) {
    return parseflo_html_DomNode(htmlString, currentPosition, prevNodeID,
                                 newNodeID, isSingle, context, false, dom,
                                 textStore);
}

flo_html_DomStatus
parseExclamdomNode(const flo_html_String htmlString, size_t *currentPosition,
                   flo_html_node_id *prevNodeID, flo_html_node_id *newNodeID,
                   flo_html_Dom *dom, flo_html_TextStore *textStore) {
    bool ignore = 0;
    TextParsing ignore2 = BASIC_CONTEXT;
    return parseflo_html_DomNode(htmlString, currentPosition, prevNodeID,
                                 newNodeID, &ignore, &ignore2, 1, dom,
                                 textStore);
}

unsigned char textNodeContinue(const char ch, const flo_html_String htmlString,
                               const size_t currentPosition) {
    return (ch != '\0' && !flo_html_isSpecialSpace(ch) &&
            (ch != ' ' ||
             (currentPosition > 0 &&
              flo_html_getChar(htmlString, currentPosition - 1) != ' ')));
}

flo_html_DomStatus
parseTextNode(const flo_html_String htmlString, size_t *currentPosition,
              flo_html_node_id *prevNodeID, flo_html_node_id *lastParsedNodeID,
              TextParsing *context, bool *isMerge, flo_html_Dom *dom,
              flo_html_TextStore *textStore) {
    flo_html_ElementStatus elementStatus = ELEMENT_SUCCESS;
    flo_html_DomStatus documentStatus = DOM_SUCCESS;
    size_t elementStartIndex = *currentPosition;
    unsigned char ch = flo_html_getChar(htmlString, *currentPosition);
    size_t elementLen = 0;

    // Continue until we encounter extra space or the end of the text
    // node.
    switch (*context) {
    case BASIC_CONTEXT: {
        while (textNodeContinue(ch, htmlString, *currentPosition) &&
               ch != '<') {
            ch = flo_html_getChar(htmlString, ++(*currentPosition));
        }

        elementLen = *currentPosition - elementStartIndex;
        if (*currentPosition > 0 &&
            flo_html_getChar(htmlString, (*currentPosition) - 1) == ' ') {
            if (elementLen > 0) {
                elementLen--;
            }
        }
        break;
    }
    case STYLE_CONTEXT: {
        const flo_html_String styleTag = FLO_HTML_S("</style");
        while (textNodeContinue(ch, htmlString, *currentPosition) &&
               (*currentPosition + styleTag.len <= htmlString.len ||
                !flo_html_stringEquals(
                    FLO_HTML_S_LEN(
                        flo_html_getCharPtr(htmlString, *currentPosition),
                        styleTag.len),
                    styleTag))) {
            ch = flo_html_getChar(htmlString, ++(*currentPosition));
        }

        elementLen = *currentPosition - elementStartIndex;
        if (*currentPosition > 0 &&
            flo_html_getChar(htmlString, *currentPosition - 1) == ' ') {
            if (elementLen > 0) {
                elementLen--;
            }
        }

        if (*currentPosition + styleTag.len >= htmlString.len &&
            flo_html_stringEquals(
                FLO_HTML_S_LEN(
                    flo_html_getCharPtr(htmlString, *currentPosition),
                    styleTag.len),
                styleTag)) {
            *context = BASIC_CONTEXT;
            if (elementLen < 1) {
                return documentStatus;
            }
        }

        break;
    }
    case SCRIPT_CONTEXT: {
        unsigned char isInString = 0;
        const flo_html_String scriptTag = FLO_HTML_S("</script");
        while (textNodeContinue(ch, htmlString, *currentPosition) &&
               (isInString ||

                (*currentPosition + scriptTag.len <= htmlString.len ||
                 !flo_html_stringEquals(
                     FLO_HTML_S_LEN(
                         flo_html_getCharPtr(htmlString, *currentPosition),
                         scriptTag.len),
                     scriptTag)))) {
            if (ch == '\'' || ch == '"' || ch == '`') {
                if (isInString == ch) {
                    isInString = 0;
                } else if (!isInString) {
                    isInString = ch;
                }
            }
            ch = flo_html_getChar(htmlString, ++(*currentPosition));
        }

        elementLen = *currentPosition - elementStartIndex;

        if (*currentPosition > 0 &&
            flo_html_getChar(htmlString, (*currentPosition) - 1) == ' ') {
            if (elementLen > 0) {
                elementLen--;
            }
        }

        if (*currentPosition + scriptTag.len >= htmlString.len &&
            flo_html_stringEquals(
                FLO_HTML_S_LEN(
                    flo_html_getCharPtr(htmlString, *currentPosition),
                    scriptTag.len),
                scriptTag)) {
            *context = BASIC_CONTEXT;
            if (elementLen < 1) {
                return documentStatus;
            }
        }

        break;
    }
    case ROGUE_OPEN_TAG: {
        // Always consume the rogue opening tag.
        ch = flo_html_getChar(htmlString, ++(*currentPosition));
        while (textNodeContinue(ch, htmlString, *currentPosition) &&
               ch != '<') {
            ch = flo_html_getChar(htmlString, ++(*currentPosition));
        }

        elementLen = *currentPosition - elementStartIndex;
        if (*currentPosition > 0 &&
            flo_html_getChar(htmlString, (*currentPosition) - 1) == ' ') {
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
        elementStatus = flo_html_addTextToTextNode(
            prevNode,
            FLO_HTML_S_LEN(flo_html_getCharPtr(htmlString, elementStartIndex),
                           elementLen),
            dom, textStore, true);
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
        elementStatus = flo_html_insertElement(
            &textStore->text,
            FLO_HTML_S_LEN(flo_html_getCharPtr(htmlString, elementStartIndex),
                           elementLen),
            &dataLocation);
        if (elementStatus != ELEMENT_CREATED) {
            FLO_HTML_ERROR_WITH_CODE_ONLY(
                flo_html_elementStatusToString(elementStatus),
                "Failed to index text!\n");
            return DOM_NO_ELEMENT;
        }

        flo_html_setNodeText(*lastParsedNodeID,
                             FLO_HTML_S_LEN(dataLocation, elementLen), dom);
    }

    return documentStatus;
}

flo_html_DomStatus flo_html_parse(const flo_html_String htmlString,
                                  flo_html_Dom *dom,
                                  flo_html_TextStore *textStore) {
    flo_html_DomStatus documentStatus = DOM_SUCCESS;

    size_t currentPosition = 0;

    flo_html_NodeDepth nodeStack;
    nodeStack.len = 0;

    TextParsing context = BASIC_CONTEXT;

    flo_html_node_id prevNodeID = 0;
    flo_html_node_id lastParsedNodeID = 0;
    unsigned char ch = flo_html_getChar(htmlString, currentPosition);

    while (ch != '\0') {
        ch = flo_html_getChar(htmlString, currentPosition);
        while (ch == ' ' || flo_html_isSpecialSpace(ch)) {
            ch = flo_html_getChar(htmlString, ++currentPosition);
        }
        if (ch == '\0') {
            break;
        }

        // Text node.
        if (context != BASIC_CONTEXT || ch != '<' ||
            (ch == '<' &&
             (flo_html_getChar(htmlString, currentPosition + 1) == ' ' ||
              flo_html_isSpecialSpace(
                  flo_html_getChar(htmlString, currentPosition + 1))))) {
            if (context == BASIC_CONTEXT && ch == '<' &&
                (flo_html_getChar(htmlString, currentPosition + 1) == ' ' ||
                 flo_html_isSpecialSpace(
                     flo_html_getChar(htmlString, currentPosition + 1)))) {
                context = ROGUE_OPEN_TAG;
            }
            bool isMerge = 0;

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
            if (flo_html_getChar(htmlString, currentPosition + 1) == '/') {
                while (ch != '\0' && ch != '>') {
                    ch = flo_html_getChar(htmlString, ++currentPosition);
                }
                if (ch != '\0') {
                    ch = flo_html_getChar(htmlString, ++currentPosition);
                }

                if (nodeStack.len > 0) {
                    nodeStack.len--;
                    lastParsedNodeID = nodeStack.stack[nodeStack.len];
                }
                context = BASIC_CONTEXT;
            }
            // Comments or <!DOCTYPE>.
            else if (flo_html_getChar(htmlString, currentPosition + 1) == '!') {
                // Skip comments.
                if (flo_html_getChar(htmlString, currentPosition + 2) == '-' &&
                    flo_html_getChar(htmlString, currentPosition + 3) == '-') {
                    while (ch != '\0' &&
                           (ch != '>' ||
                            (currentPosition >= 1 &&
                             flo_html_getChar(htmlString,
                                              currentPosition - 1) != '-') ||
                            (currentPosition >= 2 &&
                             flo_html_getChar(htmlString,
                                              currentPosition - 2) != '-'))) {
                        ch = flo_html_getChar(htmlString, ++currentPosition);
                    }
                    if (ch != '\0') {
                        ch = flo_html_getChar(htmlString, ++currentPosition);
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
                bool isSingle = 0;
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
        documentNode->tag, *newNodeID, documentNode->isPaired, dom, textStore);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add tag to new node ID!\n");
        return domStatus;
    }

    for (size_t i = 0; i < documentNode->boolPropsLen; i++) {
        const flo_html_String boolProp = documentNode->boolProps[i];
        flo_html_ElementStatus elementStatus =
            flo_html_addBooleanPropertyToNode(*newNodeID, boolProp, dom,
                                              textStore);
        if (elementStatus != ELEMENT_SUCCESS) {
            FLO_HTML_PRINT_ERROR(
                "Failed to boolean property to new node ID!\n");
            return DOM_NO_ELEMENT;
        }
    }

    for (size_t i = 0; i < documentNode->propsLen; i++) {
        const flo_html_String keyProp = documentNode->keyProps[i];
        const flo_html_String valueProp = documentNode->valueProps[i];
        flo_html_ElementStatus elementStatus = flo_html_addPropertyToNode(
            *newNodeID, keyProp, valueProp, dom, textStore);
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
    flo_html_ElementStatus elementStatus =
        flo_html_insertElement(&textStore->text, text, &dataLocation);
    if (elementStatus != ELEMENT_CREATED) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(elementStatus),
            "Failed to index text!\n");
        return DOM_NO_ELEMENT;
    }

    flo_html_setNodeText(*newNodeID, FLO_HTML_S_LEN(dataLocation, text.len),
                         dom);

    return domStatus;
}
