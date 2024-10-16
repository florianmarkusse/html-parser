#include "flo/html-parser/parser.h"
#include "error.h"
#include "flo/html-parser/definitions.h"
#include "flo/html-parser/dom/appendix.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/node/node.h"
#include "flo/html-parser/util/parse.h"
#include "flo/util/log.h"
#include "flo/util/memory/arena.h"
#include "flo/util/raw-data.h"
#include "flo/util/text/char.h"

#define HTML_COMMENT_START_LENGTH 4

flo_String voidElementTags[] = {
    FLO_STRING("area"), FLO_STRING("base"),   FLO_STRING("br"),
    FLO_STRING("col"),  FLO_STRING("embed"),  FLO_STRING("hr"),
    FLO_STRING("img"),  FLO_STRING("input"),  FLO_STRING("link"),
    FLO_STRING("meta"), FLO_STRING("source"), FLO_STRING("track"),
    FLO_STRING("wbr")};

bool isVoidElement(flo_String str) {
    for (ptrdiff_t i = 0;
         i < (ptrdiff_t)(sizeof(voidElementTags) / sizeof(voidElementTags[0]));
         i++) {
        if (flo_stringEquals(str, voidElementTags[i])) {
            return true;
        }
    }
    return false;
}

static void updateReferences(flo_html_node_id newNodeID,
                             flo_html_node_id previousNodeID,
                             flo_html_NodeDepth *depthStack, flo_html_Dom *dom,
                             flo_Arena *perm) {
    if (newNodeID > 0 && previousNodeID > 0) {
        if (depthStack->len == 0) {
            *FLO_PUSH(&dom->nextNodes, perm) = (flo_html_NextNode){
                .currentNodeID = previousNodeID, .nextNodeID = newNodeID};
        } else {
            flo_html_node_id parentNodeID =
                depthStack->stack[depthStack->len - 1].nodeID;
            *FLO_PUSH(&dom->parentChilds, perm) = (flo_html_ParentChild){
                .parentID = parentNodeID, .childID = newNodeID};
            if (parentNodeID == previousNodeID) {
                *FLO_PUSH(&dom->parentFirstChilds, perm) =
                    (flo_html_ParentChild){.parentID = parentNodeID,
                                           .childID = newNodeID};
            } else {
                *FLO_PUSH(&dom->nextNodes, perm) = (flo_html_NextNode){
                    .currentNodeID = previousNodeID, .nextNodeID = newNodeID};
            }
        }
    }
}

unsigned char textNodeContinue(flo_parse_Status ps, unsigned char ch) {
    return (
        !flo_isFormattingCharacter(ch) &&
        (ch != ' ' || (ps.idx > 0 && flo_getChar(ps.text, ps.idx - 1) != ' ')));
}

typedef struct {
    bool canHaveChildren;
    flo_String tag;
    flo_html_node_id nodeID;
    ptrdiff_t nextPosition;
} flo_html_NodeParseResult;

static inline bool isCommentTag(flo_parse_Status ps) {
    return ps.idx < ps.text.len - HTML_COMMENT_START_LENGTH &&
           flo_stringEquals(FLO_STRING("!--"),
                            FLO_STRING_LEN(flo_getCharPtr(ps.text, ps.idx + 1),
                                           HTML_COMMENT_START_LENGTH - 1));
}

#define CDATA_LENGTH 9

static inline bool isCloseTag(flo_parse_Status ps, flo_String containingTag) {
    return ps.idx < ps.text.len - containingTag.len - 2 &&
           flo_stringEquals(
               FLO_STRING("/"),
               FLO_STRING_LEN(flo_getCharPtr(ps.text, ps.idx + 1), 1)) &&
           flo_stringEquals(containingTag,
                            FLO_STRING_LEN(flo_getCharPtr(ps.text, ps.idx + 2),
                                           containingTag.len));
}

static inline bool isEndDocumentTag(flo_parse_Status ps,
                                    flo_String closeToken) {
    ps.idx++;
    if (flo_parse_currentChar(ps) == '/' &&
        ps.idx + closeToken.len < ps.text.len - 1) {
        ps.idx++;
        flo_String possibleCloseToken =
            FLO_STRING_LEN(flo_getCharPtr(ps.text, ps.idx), closeToken.len);
        return flo_stringEquals(possibleCloseToken, closeToken);
    }

    return false;
}

typedef struct {
    flo_html_node_id newNodeID;
    ptrdiff_t nextIndex;
} flo_html_TextNodeParseResult;

flo_html_TextNodeParseResult parseTextNode(flo_parse_Status ps,
                                           flo_html_Dom *dom,
                                           flo_String containingTag,
                                           flo_Arena *perm) {
    flo_html_node_id nodeID = flo_html_createNode(NODE_TYPE_TEXT, dom, perm);

    flo_RawData raw = {0};
    flo_String whiteSpace = FLO_STRING(" ");
    bool isAppend = false;

    bool isSpecialTag = flo_stringEquals(containingTag, FLO_STRING("script")) ||
                        flo_stringEquals(containingTag, FLO_STRING("style"));

    FLO_PARSE_PARSE_CHAR_UNTIL(
        ps,
        (ch == '<' && (!isCommentTag(ps) ||
                       (isSpecialTag && isCloseTag(ps, containingTag)))),
        {
            if (ch == '<' && isCommentTag(ps)) {
                FLO_PARSE_SKIP_COMMENT(ps);
            } else {
                ptrdiff_t textStart = ps.idx;
                ptrdiff_t textLen = 0;
                FLO_PARSE_NEXT_CHAR_WHILE(
                    ps, textNodeContinue(ps, ch) && ch != '<', { textLen++; })

                // If we encountered '  ' we want to subtract 1 from the
                // length.
                if (ps.idx > 0 && flo_getChar(ps.text, ps.idx - 1) == ' ') {
                    textLen--;
                }

                if (isAppend) {
                    flo_addRawData(&raw, whiteSpace, perm);
                }
                isAppend = true;

                flo_addRawData(
                    &raw,
                    FLO_STRING_LEN(flo_getCharPtr(ps.text, textStart), textLen),
                    perm);
            }
            FLO_PARSE_SKIP_EMPTY_SPACE(ps);
        })

    dom->nodes.buf[nodeID].text = FLO_STRING_LEN(raw.buf, raw.len);

    return (flo_html_TextNodeParseResult){.newNodeID = nodeID,
                                          .nextIndex = ps.idx};
}

flo_String parseProp(flo_parse_Status *ps) {
    ptrdiff_t propStart = ps->idx;

    unsigned char ch = flo_parse_currentChar(*ps);
    ptrdiff_t propLen;
    if (ch == '\'' || ch == '"') {
        unsigned char quote = ch;

        ptrdiff_t quoteCount = 0;
        propLen = -2; // 2 quotes so we start at -2.
        FLO_PARSE_NEXT_CHAR_WHILE(*ps, quoteCount < 2, {
            quoteCount += (ch == quote);
            propLen++;
        })
        propStart++;
    } else {
        propLen = 0;
        FLO_PARSE_NEXT_CHAR_UNTIL(*ps,
                                  ch == ' ' || flo_isFormattingCharacter(ch) ||
                                      ch == '=' || ch == '>',
                                  { propLen++; })
    }
    FLO_ASSERT(propLen >= 0);

    return FLO_STRING_LEN(flo_getCharPtr(ps->text, propStart), propLen);
}

flo_html_NodeParseResult parseDocumentNode(flo_parse_Status ps, bool exclamTag,
                                           flo_html_Dom *dom, flo_Arena *perm) {
    flo_html_NodeParseResult result;
    flo_html_node_id nodeID =
        flo_html_createNode(NODE_TYPE_DOCUMENT, dom, perm);

    ptrdiff_t tagStart = ps.idx;
    FLO_PARSE_NEXT_CHAR_UNTIL(ps, ch == '>' || ch == ' ' ||
                                      flo_isFormattingCharacter(ch));
    ptrdiff_t tagSize = ps.idx - tagStart;

    bool canHaveChildren = !exclamTag;

    // For example <input/>.
    if (flo_parse_currentChar(ps) == '>' &&
        flo_getChar(ps.text, ps.idx - 1) == '/') {
        canHaveChildren = false;
        tagSize--;
    }

    flo_String documentTag =
        FLO_STRING_LEN(flo_getCharPtr(ps.text, tagStart), tagSize);
    // For example <br>.
    if (isVoidElement(documentTag)) {
        canHaveChildren = false;
    }

    FLO_PARSE_SKIP_EMPTY_SPACE(ps);

    FLO_PARSE_PARSE_CHAR_UNTIL(ps, ch == '>', {
        if (ch == '/') {
            canHaveChildren = false;
            ps.idx++;
        } else if (ch != ' ' && !flo_isFormattingCharacter(ch)) {
            // Found a property.
            // Not sure yet if it is a boolean or key-value property.
            // Accepted values:
            // 'property' / "property" / property
            flo_String propKey = parseProp(&ps);
            // Check for key-value property
            if (flo_parse_currentChar(ps) == '=') {
                // Expected syntax: key="value" OR
                // Expected syntax: key='value' OR
                // Expected syntax: key=value (This is invalid html, but
                // will still support it.)
                ps.idx++;
                flo_String propValue = parseProp(&ps);
                flo_html_addPropertyToNode(nodeID, propKey, propValue, dom,
                                           perm);
            } else {
                flo_html_addBooleanPropertyToNode(nodeID, propKey, dom, perm);
            }
        }
        FLO_PARSE_SKIP_EMPTY_SPACE(ps);
    })

    flo_html_setTagOnDocumentNode(documentTag, nodeID, canHaveChildren, dom,
                                  perm);

    ps.idx++; // Skip '>', we basically already parsed it by adding
              // the tag to the node ID.

    result.nodeID = nodeID;
    result.canHaveChildren = canHaveChildren;
    result.tag = documentTag;
    result.nextPosition = ps.idx;

    return result;
}

flo_html_Dom *flo_html_parse(flo_String html, flo_html_Dom *dom,
                             flo_html_node_id prevNodeID,
                             flo_html_NodeDepth *nodeStack, flo_Arena *perm) {
    flo_parse_Status ps = {.idx = 0, .text = html};
    ptrdiff_t nodeStartLen = nodeStack->len;

    FLO_PARSE_SKIP_EMPTY_SPACE(ps);

    while (ps.idx < ps.text.len) {
        if (flo_parse_currentChar(ps) == '<') {
            if (isCommentTag(ps)) {
                FLO_PARSE_SKIP_COMMENT(ps)
            } else if (isEndDocumentTag(
                           ps, nodeStack->stack[nodeStack->len - 1].tag)) {
                if (nodeStack->len > nodeStartLen) {
                    nodeStack->len--;
                    prevNodeID = nodeStack->stack[nodeStack->len].nodeID;
                }
                FLO_PARSE_NEXT_CHAR_UNTIL(ps, (ch == '>'));
                ps.idx++;
            } else {
                ps.idx++;
                unsigned char ch = flo_parse_currentChar(ps);

                if (flo_isAlphabetical(ch) || ch == '!') {
                    // standard opening tag or !DOCTYPE and friends
                    flo_html_NodeParseResult parseResult =
                        parseDocumentNode(ps, ch == '!', dom, perm);

                    updateReferences(parseResult.nodeID, prevNodeID, nodeStack,
                                     dom, perm);

                    if (parseResult.canHaveChildren) {
                        if (nodeStack->len >= FLO_HTML_MAX_NODE_DEPTH) {
                            FLO_ERROR("Reached max node depth\n");
                            return NULL;
                        }
                        nodeStack->stack[nodeStack->len].nodeID =
                            parseResult.nodeID;
                        nodeStack->stack[nodeStack->len].tag = parseResult.tag;
                        nodeStack->len++;
                    }
                    prevNodeID = parseResult.nodeID;
                    ps.idx = parseResult.nextPosition;
                } else {
                    // Rogue open tag -> Text node.
                    flo_html_TextNodeParseResult parseResult = parseTextNode(
                        ps, dom,
                        nodeStack->len > 0
                            ? nodeStack->stack[nodeStack->len - 1].tag
                            : FLO_EMPTY_STRING,
                        perm);
                    updateReferences(parseResult.newNodeID, prevNodeID,
                                     nodeStack, dom, perm);
                    prevNodeID = parseResult.newNodeID;
                    ps.idx = parseResult.nextIndex;
                }
            }
        }
        // Text node.
        else {
            flo_html_TextNodeParseResult parseResult = parseTextNode(
                ps, dom,
                nodeStack->len > 0 ? nodeStack->stack[nodeStack->len - 1].tag
                                   : FLO_EMPTY_STRING,
                perm);
            updateReferences(parseResult.newNodeID, prevNodeID, nodeStack, dom,
                             perm);
            prevNodeID = parseResult.newNodeID;
            ps.idx = parseResult.nextIndex;
        }

        FLO_PARSE_SKIP_EMPTY_SPACE(ps);
    }

    return dom;
}

flo_html_Dom *flo_html_parseRoot(flo_String html, flo_html_Dom *dom,
                                 flo_Arena *perm) {
    flo_html_NodeDepth nodeStack;
    nodeStack.stack[0].nodeID = FLO_HTML_ROOT_NODE_ID;
    nodeStack.stack[0].tag = FLO_EMPTY_STRING;
    nodeStack.len = 1;

    return flo_html_parse(html, dom, FLO_HTML_ROOT_NODE_ID, &nodeStack, perm);
}

flo_html_Dom *flo_html_parseExtra(flo_String html, flo_html_Dom *dom,
                                  flo_Arena *perm) {
    flo_html_NodeDepth nodeStack;
    nodeStack.len = 0;

    return flo_html_parse(html, dom, FLO_HTML_ERROR_NODE_ID, &nodeStack, perm);
}

flo_html_node_id
flo_html_parseDocumentElement(flo_html_DocumentNode *documentNode,
                              flo_html_Dom *dom, flo_Arena *perm) {
    flo_html_node_id newNodeID =
        flo_html_createNode(NODE_TYPE_DOCUMENT, dom, perm);

    flo_html_setTagOnDocumentNode(documentNode->tag, newNodeID,
                                  documentNode->isPaired, dom, perm);

    for (ptrdiff_t i = 0; i < documentNode->boolPropsLen; i++) {
        flo_String boolProp = documentNode->boolProps[i];
        flo_html_addBooleanPropertyToNode(newNodeID, boolProp, dom, perm);
    }

    for (ptrdiff_t i = 0; i < documentNode->propsLen; i++) {
        flo_String keyProp = documentNode->keyProps[i];
        flo_String valueProp = documentNode->valueProps[i];
        flo_html_addPropertyToNode(newNodeID, keyProp, valueProp, dom, perm);
    }

    return newNodeID;
}

flo_html_node_id flo_html_parseTextElement(flo_String text, flo_html_Dom *dom,
                                           flo_Arena *perm) {
    flo_html_node_id newNodeID = flo_html_createNode(NODE_TYPE_TEXT, dom, perm);
    unsigned char *malloced = FLO_NEW(perm, unsigned char, text.len);
    memcpy(malloced, text.buf, text.len);
    dom->nodes.buf[newNodeID].text = FLO_STRING_LEN(malloced, text.len);

    return newNodeID;
}
