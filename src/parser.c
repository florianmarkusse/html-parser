#include <ctype.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/dom/appendix.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/node/node.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/type/element/elements-print.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"
#include "flo/html-parser/util/text/char.h"

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
    for (ptrdiff_t i = 0;
         i < sizeof(voidElementTags) / sizeof(voidElementTags[0]); i++) {
        if (flo_html_stringEquals(str, voidElementTags[i])) {
            return true;
        }
    }
    return false; // No match found
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
                depthStack->stack[depthStack->len - 1].nodeID;
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

#define UPDATE_REFERENCES(parseResult, prevNodeID, nodeStack, dom)             \
    do {                                                                       \
        if ((documentStatus =                                                  \
                 updateReferences((parseResult).nodeID, (prevNodeID),          \
                                  &(nodeStack), (dom))) != DOM_SUCCESS) {      \
            FLO_HTML_PRINT_ERROR("Failed to update references.\n");            \
            return documentStatus;                                             \
        }                                                                      \
    } while (0)

#define PARSE_TEXT_NODE(parseResult, html, currentPosition, parentTag, dom,    \
                        textStore)                                             \
    do {                                                                       \
        (parseResult) = parseTextNode((html), (currentPosition), (parentTag),  \
                                      (dom), (textStore));                     \
        if ((parseResult).status != DOM_SUCCESS) {                             \
            FLO_HTML_PRINT_ERROR("Failed to parse text node.\n");              \
            return (parseResult).status;                                       \
        }                                                                      \
    } while (0)

unsigned char textNodeContinue(const char ch, const flo_html_String htmlString,
                               const ptrdiff_t currentPosition) {
    return (!flo_html_isSpecialSpace(ch) &&
            (ch != ' ' ||
             (currentPosition > 0 &&
              flo_html_getChar(htmlString, currentPosition - 1) != ' ')));
}

typedef struct {
    flo_html_DomStatus status;
    bool canHaveChildren;
    flo_html_String tag;
    flo_html_node_id nodeID;
    ptrdiff_t nextPosition;
} flo_html_NodeParseResult;

flo_html_NodeParseResult parseCloseTag(flo_html_String html, ptrdiff_t start) {
    flo_html_NodeParseResult result;
    ptrdiff_t end = start;
    unsigned char ch = flo_html_getChar(html, end);
    while (end < html.len && ch != '>') {
        ch = flo_html_getChar(html, ++end);
    }

    result.status = DOM_SUCCESS;
    if (end < html.len) {
        end++;
    }
    result.nextPosition = end;

    return result;
}

static ptrdiff_t parseEmptyContent(flo_html_String html, ptrdiff_t start) {
    ptrdiff_t end = start;
    unsigned char ch = flo_html_getChar(html, end);
    while (end < html.len && (ch == ' ' || flo_html_isSpecialSpace(ch))) {
        ch = flo_html_getChar(html, ++end);
    }

    return end;
}

flo_html_NodeParseResult parseComment(flo_html_String html, ptrdiff_t start) {
    flo_html_NodeParseResult result;
    ptrdiff_t end = start;
    unsigned char ch = flo_html_getChar(html, end);
    while (end < html.len &&
           !(ch == '>' &&
             flo_html_stringEquals(
                 FLO_HTML_S("--"),
                 FLO_HTML_S_LEN(flo_html_getCharPtr(html, end - 2), 2)))) {
        ch = flo_html_getChar(html, ++end);
    }

    result.status = DOM_SUCCESS;
    if (end < html.len) {
        end++;
    }
    result.nextPosition = end;

    return result;
}

bool isCommentTag(const flo_html_String html, ptrdiff_t currentPosition) {
    return currentPosition < html.len - 3 &&
           flo_html_stringEquals(
               FLO_HTML_S("!--"),
               FLO_HTML_S_LEN(flo_html_getCharPtr(html, currentPosition + 1),
                              3));
}

bool isEndOfTextNode(const flo_html_String html, ptrdiff_t currentIndex,
                     flo_html_String closeToken) {
    unsigned char nextCh = flo_html_getChar(html, currentIndex + 1);

    if ((flo_html_isAlphaBetical(nextCh) || nextCh == '!' || nextCh == '/') &&
        currentIndex + closeToken.len <= html.len) {
        flo_html_String possibleCloseToken = FLO_HTML_S_LEN(
            flo_html_getCharPtr(html, currentIndex + 1), closeToken.len);
        return flo_html_stringEquals(possibleCloseToken, closeToken);
    }

    return false;
}

flo_html_NodeParseResult parseTextNode(const flo_html_String html,
                                       ptrdiff_t textStart,
                                       flo_html_String parentTag,
                                       flo_html_Dom *dom,
                                       flo_html_TextStore *textStore) {
    flo_html_NodeParseResult result;
    flo_html_DomStatus documentStatus;
    flo_html_node_id nodeID;

    if ((documentStatus = flo_html_createNode(&nodeID, NODE_TYPE_TEXT, dom)) !=
        DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to create node.\n");
        result.status = documentStatus;
        return result;
    }

    ptrdiff_t textEnd =
        textStart + 1; // Always consume at least a single character, otherwise
                       // problematic if we have a roque opening tag.
    unsigned char ch = flo_html_getChar(html, textEnd);

    ptrdiff_t totalTextLen = 0;
    ptrdiff_t maxTextSize = 1U << 17U;
    // TODO: transform into scratch arena allocator.
    unsigned char *textBuffer = malloc(maxTextSize);
    if (textBuffer == NULL) {
        FLO_HTML_PRINT_ERROR("Failed to allocate memory for text buffer.\n");
        result.status = DOM_ERROR_MEMORY;
        return result;
    }
    flo_html_String textString;
    textString.buf = textBuffer;

    flo_html_String closeToken = FLO_HTML_EMPTY_STRING;
    if (flo_html_stringEquals(parentTag, FLO_HTML_S("style"))) {
        closeToken = FLO_HTML_S("/style");
    }
    if (flo_html_stringEquals(parentTag, FLO_HTML_S("script"))) {
        closeToken = FLO_HTML_S("/script");
    }

    flo_html_NodeParseResult commentNode;
    // Continue until the end of the text node.
    while (textEnd < html.len &&
           !(ch == '<' && isEndOfTextNode(html, textEnd, closeToken) &&
             !isCommentTag(html, textEnd))) {
        if (ch == '<' && isCommentTag(html, textEnd)) {
            commentNode = parseComment(html, textEnd + 4);
            textStart = parseEmptyContent(html, commentNode.nextPosition);
        } else {
            // Continue until we encounter extra space or the end of the text
            // node.
            while (textEnd < html.len && textNodeContinue(ch, html, textEnd) &&
                   !(ch == '<' && isEndOfTextNode(html, textEnd, closeToken))) {
                ch = flo_html_getChar(html, ++(textEnd));
            }

            ptrdiff_t textLen = textEnd - textStart;
            if (flo_html_getChar(html, textEnd - 1) == ' ') {
                textLen--;
            }

            if (textLen + totalTextLen + 1 > maxTextSize) {
                FLO_HTML_PRINT_ERROR(
                    "Text node is too large to fit into text buffer.\n");
                FLO_HTML_PRINT_ERROR(
                    "want to add \n%.*s\n",
                    FLO_HTML_S_P(FLO_HTML_S_LEN(
                        flo_html_getCharPtr(html, textStart), textLen)));
                FLO_HTML_PRINT_ERROR(
                    "current text is \n%.*s\n",
                    FLO_HTML_S_P(FLO_HTML_S_LEN(textBuffer, totalTextLen)));
                FLO_HTML_FREE_TO_NULL(textBuffer);
                result.status = DOM_ERROR_MEMORY;
                return result;
            }

            textBuffer[totalTextLen] = ' ';
            memcpy(&textBuffer[totalTextLen + (totalTextLen > 0)],
                   flo_html_getCharPtr(html, textStart), textLen);
            totalTextLen += textLen + (totalTextLen > 0);

            textStart = parseEmptyContent(html, textEnd);
        }
        textEnd = textStart;
        ch = flo_html_getChar(html, textEnd);
    }
    textString.len = totalTextLen;

    char *dataLocation = NULL;
    flo_html_ElementStatus status =
        flo_html_insertElement(&textStore->text, textString, &dataLocation);
    if (status != ELEMENT_CREATED) {
        FLO_HTML_FREE_TO_NULL(textBuffer);
        FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_elementStatusToString(status),
                                      "Failed to index text!\n");
        result.status = DOM_NO_ELEMENT;
        return result;
    }
    FLO_HTML_FREE_TO_NULL(textBuffer);

    flo_html_setNodeText(nodeID, FLO_HTML_S_LEN(dataLocation, textString.len),
                         dom);

    result.nodeID = nodeID;
    result.status = DOM_SUCCESS;
    result.canHaveChildren = false;
    result.nextPosition = textEnd;

    return result;
}

ptrdiff_t parseProp(const flo_html_String html, ptrdiff_t propStart,
                    flo_html_String *prop) {
    ptrdiff_t propEnd = propStart;
    ptrdiff_t nextChar = propStart;
    unsigned ch = flo_html_getChar(html, propEnd);

    if (ch == '\'' || ch == '"') {
        unsigned char quote = ch;

        ptrdiff_t quoteCount = 0;
        while (propEnd < html.len && quoteCount < 2) {
            quoteCount += (ch == quote); // Be gone branches.
            ch = flo_html_getChar(html, ++nextChar);
        }

        propStart++;
        propEnd = nextChar - 1;
    } else {
        while (propEnd < html.len && ch != ' ' &&
               !flo_html_isSpecialSpace(ch) && ch != '>' && ch != '=') {
            ch = flo_html_getChar(html, ++nextChar);
        }
        propEnd = nextChar;
    }
    ptrdiff_t propLen = propEnd - propStart;
    *prop = FLO_HTML_S_LEN(flo_html_getCharPtr(html, propStart), propLen);

    return nextChar;
}

flo_html_NodeParseResult parseDocumentNode(const flo_html_String html,
                                           ptrdiff_t tagStart, bool exclamTag,
                                           flo_html_Dom *dom,
                                           flo_html_TextStore *textStore) {
    flo_html_NodeParseResult result;
    flo_html_DomStatus documentStatus;
    flo_html_node_id nodeID;

    if ((documentStatus = flo_html_createNode(&nodeID, NODE_TYPE_DOCUMENT,
                                              dom)) != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to create node.\n");
        result.status = documentStatus;
        return result;
    }

    ptrdiff_t parsedChars = tagStart;
    unsigned char ch = flo_html_getChar(html, parsedChars);
    // Read until end of tag (mostly, we may do some correcting afterwards)
    while (parsedChars < html.len && ch != '>' && ch != ' ' &&
           !flo_html_isSpecialSpace(ch)) {
        ch = flo_html_getChar(html, ++(parsedChars));
    }
    ptrdiff_t tagSize = parsedChars - tagStart;

    bool canHaveChildren = !exclamTag;

    // For example <input/>.
    if (ch == '>' && flo_html_getChar(html, parsedChars - 1) == '/') {
        canHaveChildren = false;
        tagSize--;
    }

    flo_html_String documentTag =
        FLO_HTML_S_LEN(flo_html_getCharPtr(html, tagStart), tagSize);
    // For example <br>.
    if (isVoidElement(documentTag)) {
        canHaveChildren = false;
    }

    parsedChars = parseEmptyContent(html, parsedChars);
    ch = flo_html_getChar(html, parsedChars);
    while (parsedChars < html.len && ch != '>') {
        if (ch == '/') {
            canHaveChildren = false;
            parsedChars++;
        }
        // Found a property.
        // Not sure yet if it is a boolean or key-value property.
        // Accepted values:
        // 'property' / "property" / property
        else {
            flo_html_String propKey;
            parsedChars = parseProp(html, parsedChars, &propKey);
            ch = flo_html_getChar(html, parsedChars);
            // Check for key-value property
            if (ch == '=') {
                // Expected syntax: key="value" OR
                // Expected syntax: key='value' OR
                // Expected syntax: key=value (This is invalid html, but
                // will still support it) We can do some more interesting
                // stuff but currently not required.
                flo_html_String propValue;
                parsedChars =
                    parseProp(html, parsedChars + 1, &propValue); // skip '='
                ch = flo_html_getChar(html, parsedChars);

                flo_html_ElementStatus status = flo_html_addPropertyToNode(
                    nodeID, propKey, propValue, dom, textStore);
                if (status != ELEMENT_SUCCESS) {
                    FLO_HTML_ERROR_WITH_CODE_ONLY(
                        flo_html_elementStatusToString(status),
                        "Failed to add key-value property!\n");
                    result.status = DOM_NO_ELEMENT;
                    return result;
                }
            } else {
                flo_html_ElementStatus status =
                    flo_html_addBooleanPropertyToNode(nodeID, propKey, dom,
                                                      textStore);
                if (status != ELEMENT_SUCCESS) {
                    FLO_HTML_ERROR_WITH_CODE_ONLY(
                        flo_html_elementStatusToString(status),
                        "Failed to add boolean property!\n");
                    result.status = DOM_NO_ELEMENT;
                    return result;
                }
            }
        }
        parsedChars = parseEmptyContent(html, parsedChars);
        ch = flo_html_getChar(html, parsedChars);
    }

    if ((documentStatus =
             flo_html_setTagOnDocumentNode(documentTag, nodeID, canHaveChildren,
                                           dom, textStore)) != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add tag to node ID.\n");
        result.status = documentStatus;
        return result;
    }

    result.nodeID = nodeID;
    result.status = DOM_SUCCESS;
    result.canHaveChildren = canHaveChildren;
    result.tag = documentTag;
    result.nextPosition =
        parsedChars + 1; // Skip '>', we basically already parsed it by adding
                         // the tag to the node ID.

    return result;
}

flo_html_DomStatus flo_html_parse(const flo_html_String html, flo_html_Dom *dom,
                                  flo_html_TextStore *textStore,
                                  flo_html_NodeDepth *nodeStack) {
    flo_html_DomStatus documentStatus = DOM_SUCCESS;

    flo_html_node_id prevNodeID = dom->nodes[1].nodeID;
    ptrdiff_t nodeStartLen = nodeStack->len;

    ptrdiff_t currentPosition = parseEmptyContent(html, 0);
    unsigned char ch = flo_html_getChar(html, currentPosition);
    while (currentPosition < html.len) {
        flo_html_NodeParseResult parseResult;
        // Open document node.
        if (ch == '<' && currentPosition < html.len - 1) {
            if (isCommentTag(html, currentPosition)) {
                parseResult = parseComment(html, currentPosition + 4);
                parseResult.nodeID = prevNodeID;
            } else {
                unsigned char nextCh =
                    flo_html_getChar(html, currentPosition + 1);

                if (flo_html_isAlphaBetical(nextCh) || nextCh == '!') {
                    // standard opening tag or !DOCTYPE and friends
                    parseResult =
                        parseDocumentNode(html, currentPosition + 1,
                                          nextCh == '!', dom, textStore);
                    if (parseResult.status != DOM_SUCCESS) {
                        FLO_HTML_PRINT_ERROR(
                            "Failed to parse document node.\n");
                        return parseResult.status;
                    }

                    UPDATE_REFERENCES(parseResult, prevNodeID, *nodeStack, dom);

                    if (parseResult.canHaveChildren) {
                        if (nodeStack->len >= FLO_HTML_MAX_NODE_DEPTH) {
                            FLO_HTML_PRINT_ERROR(
                                "Reached max node depth, aborting\n");
                            return DOM_TOO_DEEP;
                        }
                        nodeStack->stack[nodeStack->len].nodeID =
                            parseResult.nodeID;
                        nodeStack->stack[nodeStack->len].tag = parseResult.tag;
                        nodeStack->len++;
                    }
                } else {
                    if (nextCh == '/') {
                        if (nodeStack->len > nodeStartLen) {
                            nodeStack->len--;
                            prevNodeID =
                                nodeStack->stack[nodeStack->len].nodeID;
                        }
                        parseResult = parseCloseTag(html, currentPosition + 2);
                        parseResult.nodeID = prevNodeID;
                    } else {
                        // Rogue open tag -> Text node.
                        PARSE_TEXT_NODE(
                            parseResult, html, currentPosition,
                            nodeStack->stack[nodeStack->len - 1].tag, dom,
                            textStore);
                        UPDATE_REFERENCES(parseResult, prevNodeID, *nodeStack,
                                          dom);
                    }
                }
            }
        }
        // Text node.
        else {
            PARSE_TEXT_NODE(parseResult, html, currentPosition,
                            nodeStack->stack[nodeStack->len - 1].tag, dom,
                            textStore);
            UPDATE_REFERENCES(parseResult, prevNodeID, *nodeStack, dom);
        }

        prevNodeID = parseResult.nodeID;
        currentPosition = parseResult.nextPosition;

        currentPosition = parseEmptyContent(html, currentPosition);
        ch = flo_html_getChar(html, currentPosition);
    }

    return documentStatus;
}

flo_html_DomStatus flo_html_parseRoot(const flo_html_String html,
                                      flo_html_Dom *dom,
                                      flo_html_TextStore *textStore) {
    flo_html_NodeDepth nodeStack;
    nodeStack.stack[0].nodeID = dom->nodes[1].nodeID;
    nodeStack.stack[0].tag = FLO_HTML_EMPTY_STRING;
    nodeStack.len = 1;

    return flo_html_parse(html, dom, textStore, &nodeStack);
}

flo_html_DomStatus flo_html_parseExtra(const flo_html_String html,
                                       flo_html_Dom *dom,
                                       flo_html_TextStore *textStore) {
    flo_html_NodeDepth nodeStack;
    nodeStack.len = 0;

    return flo_html_parse(html, dom, textStore, &nodeStack);
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

    for (ptrdiff_t i = 0; i < documentNode->boolPropsLen; i++) {
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

    for (ptrdiff_t i = 0; i < documentNode->propsLen; i++) {
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

flo_html_DomStatus flo_html_parseTextElement(flo_html_String text,
                                             flo_html_Dom *dom,
                                             flo_html_TextStore *textStore,
                                             flo_html_node_id *newNodeID) {
    flo_html_DomStatus domStatus =
        flo_html_createNode(newNodeID, NODE_TYPE_TEXT, dom);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to retrieve a new node ID!\n");
        return domStatus;
    }

    char *dataLocation;
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
