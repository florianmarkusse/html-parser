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

static void updateReferences(const flo_html_node_id newNodeID,
                             const flo_html_node_id previousNodeID,
                             const flo_html_NodeDepth *depthStack,
                             flo_html_Dom *dom) {
    if (newNodeID > 0 && previousNodeID > 0) {
        if (depthStack->len == 0) {
            flo_html_addNextNode(previousNodeID, newNodeID, dom);
        } else {
            const unsigned int parentNodeID =
                depthStack->stack[depthStack->len - 1].nodeID;
            flo_html_addParentChild(parentNodeID, newNodeID, dom);
            if (parentNodeID == previousNodeID) {
                flo_html_addParentFirstChild(parentNodeID, newNodeID, dom);
            } else {
                flo_html_addNextNode(previousNodeID, newNodeID, dom);
            }
        }
    }
}

unsigned char textNodeContinue(const char ch, const flo_html_String htmlString,
                               const ptrdiff_t currentPosition) {
    return (!flo_html_isSpecialSpace(ch) &&
            (ch != ' ' ||
             (currentPosition > 0 &&
              flo_html_getChar(htmlString, currentPosition - 1) != ' ')));
}

typedef struct {
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

flo_html_NodeParseResult
parseTextNode(const flo_html_String html, ptrdiff_t textStart,
              flo_html_String parentTag, flo_html_Dom *dom,
              flo_html_TextStore *textStore, flo_html_Arena scratch) {
    flo_html_NodeParseResult result;
    flo_html_node_id nodeID = flo_html_createNode(NODE_TYPE_TEXT, dom);

    ptrdiff_t textEnd =
        textStart + 1; // Always consume at least a single character, otherwise
                       // problematic if we have a roque opening tag.
    unsigned char ch = flo_html_getChar(html, textEnd);

    ptrdiff_t totalTextLen = 0;
    ptrdiff_t maxTextSize = 1U << 17U;
    // TODO: Can make it into a dynamic array that grows, should work much
    // better.
    // TODO: dynamic array.
    unsigned char *textBuffer =
        FLO_HTML_NEW(&scratch, unsigned char, maxTextSize);
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
                FLO_HTML_PRINT_ERROR("Implement growing of the text buffer!!!");
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

    unsigned char *dataLocation =
        flo_html_insertIntoPage(textString, &textStore->text);
    flo_html_setNodeText(nodeID, FLO_HTML_S_LEN(dataLocation, textString.len),
                         dom);

    result.nodeID = nodeID;
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
    flo_html_node_id nodeID = flo_html_createNode(NODE_TYPE_DOCUMENT, dom);

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

                flo_html_addPropertyToNode(nodeID, propKey, propValue, dom,
                                           textStore);
            } else {
                flo_html_addBooleanPropertyToNode(nodeID, propKey, dom,
                                                  textStore);
            }
        }
        parsedChars = parseEmptyContent(html, parsedChars);
        ch = flo_html_getChar(html, parsedChars);
    }

    flo_html_setTagOnDocumentNode(documentTag, nodeID, canHaveChildren, dom,
                                  textStore);

    result.nodeID = nodeID;
    result.canHaveChildren = canHaveChildren;
    result.tag = documentTag;
    result.nextPosition =
        parsedChars + 1; // Skip '>', we basically already parsed it by adding
                         // the tag to the node ID.

    return result;
}

void flo_html_parse(const flo_html_String html, flo_html_Dom *dom,
                    flo_html_TextStore *textStore,
                    flo_html_NodeDepth *nodeStack, flo_html_Arena *perm) {
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

                    updateReferences(parseResult.nodeID, prevNodeID, nodeStack,
                                     dom);

                    if (parseResult.canHaveChildren) {
                        // TODO: dynamic, but handle array growth of others
                        // first.
                        // TODO: scratch array here that can grow when too big?
                        if (nodeStack->len >= FLO_HTML_MAX_NODE_DEPTH) {
                            FLO_HTML_PRINT_ERROR("Reached max node depth\n");
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
                        parseTextNode(html, currentPosition,
                                      nodeStack->stack[nodeStack->len - 1].tag,
                                      dom, textStore, *perm);
                        updateReferences(parseResult.nodeID, prevNodeID,
                                         nodeStack, dom);
                    }
                }
            }
        }
        // Text node.
        else {
            parseTextNode(html, currentPosition,
                          nodeStack->stack[nodeStack->len - 1].tag, dom,
                          textStore, *perm);
            updateReferences(parseResult.nodeID, prevNodeID, nodeStack, dom);
        }

        prevNodeID = parseResult.nodeID;
        currentPosition = parseResult.nextPosition;

        currentPosition = parseEmptyContent(html, currentPosition);
        ch = flo_html_getChar(html, currentPosition);
    }
}

void flo_html_parseRoot(const flo_html_String html, flo_html_Dom *dom,
                        flo_html_TextStore *textStore, flo_html_Arena *perm) {
    flo_html_NodeDepth nodeStack;
    nodeStack.stack[0].nodeID = dom->nodes[1].nodeID;
    nodeStack.stack[0].tag = FLO_HTML_EMPTY_STRING;
    nodeStack.len = 1;

    flo_html_parse(html, dom, textStore, &nodeStack, perm);
}

void flo_html_parseExtra(const flo_html_String html, flo_html_Dom *dom,
                         flo_html_TextStore *textStore, flo_html_Arena *perm) {
    flo_html_NodeDepth nodeStack;
    nodeStack.len = 0;

    flo_html_parse(html, dom, textStore, &nodeStack, perm);
}

flo_html_node_id
flo_html_parseDocumentElement(const flo_html_DocumentNode *documentNode,
                              flo_html_Dom *dom,
                              flo_html_TextStore *textStore) {
    flo_html_node_id newNodeID = flo_html_createNode(NODE_TYPE_DOCUMENT, dom);

    flo_html_setTagOnDocumentNode(documentNode->tag, newNodeID,
                                  documentNode->isPaired, dom, textStore);

    for (ptrdiff_t i = 0; i < documentNode->boolPropsLen; i++) {
        const flo_html_String boolProp = documentNode->boolProps[i];
        flo_html_addBooleanPropertyToNode(newNodeID, boolProp, dom, textStore);
    }

    for (ptrdiff_t i = 0; i < documentNode->propsLen; i++) {
        const flo_html_String keyProp = documentNode->keyProps[i];
        const flo_html_String valueProp = documentNode->valueProps[i];
        flo_html_addPropertyToNode(newNodeID, keyProp, valueProp, dom,
                                   textStore);
    }

    return newNodeID;
}

flo_html_node_id flo_html_parseTextElement(flo_html_String text,
                                           flo_html_Dom *dom,
                                           flo_html_TextStore *textStore) {
    flo_html_node_id newNodeID = flo_html_createNode(NODE_TYPE_TEXT, dom);
    unsigned char *dataLocation =
        flo_html_insertIntoPage(text, &textStore->text);
    flo_html_setNodeText(newNodeID, FLO_HTML_S_LEN(dataLocation, text.len),
                         dom);

    return newNodeID;
}
