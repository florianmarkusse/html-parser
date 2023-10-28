#include "flo/html-parser/parser.h"
#include "flo/html-parser/definitions.h"
#include "flo/html-parser/dom/appendix.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/node/node.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"
#include "flo/html-parser/util/raw-data.h"
#include "flo/html-parser/util/text/char.h"

#define HTML_COMMENT_START_LENGTH 4

typedef struct {
    flo_html_String text;
    ptrdiff_t idx;
} ParseStatus;

// TODO: Do I care about these checks at all? When accessing ch, we should
// always go through a macro right?
#define NEXT_CHAR(parseStatus)                                                 \
    if ((parseStatus).idx < (parseStatus).text.len) {                          \
        ++(parseStatus).idx;                                                   \
    }

// TODO: Do I care about these checks at all? When accessing ch, we should
// always go through a macro right?
#define SKIP_CHARS(parseStatus, increase)                                      \
                                                                               \
    if ((parseStatus).idx < (parseStatus).text.len - (increase)) {             \
        (parseStatus).idx += (increase);                                       \
    }

#define NEXT_CHAR_WHILE(parseStatus, condition, body)                          \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_html_getChar((parseStatus).text, (parseStatus).idx);           \
        if (condition) {                                                       \
            { body }                                                           \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
        (parseStatus).idx++;                                                   \
    }

#define PARSE_CHAR_WHILE(parseStatus, condition, body)                         \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_html_getChar((parseStatus).text, (parseStatus).idx);           \
        if (condition) {                                                       \
            { body }                                                           \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
    }

#define NEXT_CHAR_UNTIL(parseStatus, condition, body)                          \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_html_getChar((parseStatus).text, (parseStatus).idx);           \
        if (!(condition)) {                                                    \
            { body }                                                           \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
        (parseStatus).idx++;                                                   \
    }

#define PARSE_CHAR_UNTIL(parseStatus, condition, body)                         \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_html_getChar((parseStatus).text, (parseStatus).idx);           \
        if (!(condition)) {                                                    \
            { body }                                                           \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
    }

#define SKIP_EMPTY_SPACE(parseStatus)                                          \
    NEXT_CHAR_WHILE(parseStatus, (ch == ' ' || flo_html_isSpecialSpace(ch)), {})

#define SKIP_COMMENT(parseStatus)                                              \
    SKIP_CHARS(ps, HTML_COMMENT_START_LENGTH);                                 \
    NEXT_CHAR_UNTIL(                                                           \
        ps,                                                                    \
        (ch == '>' &&                                                          \
         flo_html_stringEquals(                                                \
             FLO_HTML_S("--"),                                                 \
             FLO_HTML_S_LEN(flo_html_getCharPtr(ps.text, ps.idx - 2), 2))),    \
        {})                                                                    \
    NEXT_CHAR(ps);

static inline unsigned char currentChar(ParseStatus ps) {
    return flo_html_getChar(ps.text, ps.idx);
}

void testLoop(ParseStatus ps) {
    for (unsigned char ch; ps.idx < ps.text.len && (ch = currentChar(ps));
         ps.idx++) {
    }
}

typedef enum {
    BASIC_CONTEXT,
    SCRIPT_CONTEXT,
    STYLE_CONTEXT,
    ROGUE_OPEN_TAG
} TextParsing;

flo_html_String voidElementTags[] = {
    FLO_HTML_S("area"), FLO_HTML_S("base"),   FLO_HTML_S("br"),
    FLO_HTML_S("col"),  FLO_HTML_S("embed"),  FLO_HTML_S("hr"),
    FLO_HTML_S("img"),  FLO_HTML_S("input"),  FLO_HTML_S("link"),
    FLO_HTML_S("meta"), FLO_HTML_S("source"), FLO_HTML_S("track"),
    FLO_HTML_S("wbr")};

bool isVoidElement(flo_html_String str) {
    for (ptrdiff_t i = 0;
         i < sizeof(voidElementTags) / sizeof(voidElementTags[0]); i++) {
        if (flo_html_stringEquals(str, voidElementTags[i])) {
            return true;
        }
    }
    return false;
}

static void updateReferences(flo_html_node_id newNodeID,
                             flo_html_node_id previousNodeID,
                             flo_html_NodeDepth *depthStack, flo_html_Dom *dom,
                             flo_html_Arena *perm) {
    if (newNodeID > 0 && previousNodeID > 0) {
        if (depthStack->len == 0) {
            *FLO_HTML_PUSH(&dom->nextNodes, perm) = (flo_html_NextNode){
                .currentNodeID = previousNodeID, .nextNodeID = newNodeID};
        } else {
            unsigned int parentNodeID =
                depthStack->stack[depthStack->len - 1].nodeID;
            *FLO_HTML_PUSH(&dom->parentChilds, perm) = (flo_html_ParentChild){
                .parentID = parentNodeID, .childID = newNodeID};
            if (parentNodeID == previousNodeID) {
                *FLO_HTML_PUSH(&dom->parentFirstChilds, perm) =
                    (flo_html_ParentChild){.parentID = parentNodeID,
                                           .childID = newNodeID};
            } else {
                *FLO_HTML_PUSH(&dom->nextNodes, perm) = (flo_html_NextNode){
                    .currentNodeID = previousNodeID, .nextNodeID = newNodeID};
            }
        }
    }
}

unsigned char textNodeContinue(unsigned char ch, ParseStatus ps) {
    return (!flo_html_isSpecialSpace(ch) &&
            (ch != ' ' ||
             (ps.idx > 0 && flo_html_getChar(ps.text, ps.idx - 1) != ' ')));
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

static inline bool isCommentTag(ParseStatus ps) {
    return ps.idx < ps.text.len - HTML_COMMENT_START_LENGTH &&
           flo_html_stringEquals(
               FLO_HTML_S("!--"),
               FLO_HTML_S_LEN(flo_html_getCharPtr(ps.text, ps.idx + 1), 3));
}

static inline bool isEndDocumentTag(ParseStatus ps,
                                    flo_html_String closeToken) {
    NEXT_CHAR(ps);
    if (currentChar(ps) == '/' && ps.idx + closeToken.len < ps.text.len - 1) {
        NEXT_CHAR(ps);
        flo_html_String possibleCloseToken = FLO_HTML_S_LEN(
            flo_html_getCharPtr(ps.text, ps.idx), closeToken.len);
        return flo_html_stringEquals(possibleCloseToken, closeToken);
    }

    return false;
}

typedef struct {
    flo_html_node_id newNodeID;
    ptrdiff_t nextIndex;
} flo_html_TextNodeParseResult;

flo_html_TextNodeParseResult parseTextNode(ParseStatus ps, flo_html_Dom *dom,
                                           flo_html_Arena *perm) {
    flo_html_node_id nodeID = flo_html_createNode(NODE_TYPE_TEXT, dom, perm);

    flo_html_RawData raw = {0};
    flo_html_String whiteSpace = FLO_HTML_S(" ");
    bool isAppend = false;

    PARSE_CHAR_UNTIL(ps, (ch == '<' && !isCommentTag(ps)), {
        if (ch == '<' && isCommentTag(ps)) {
            SKIP_COMMENT(ps);
        } else {
            // Continue until we encounter extra space or the end of the
            // text node.
            ptrdiff_t textStart = ps.idx;
            ptrdiff_t textLen = 0;
            NEXT_CHAR_WHILE(ps, textNodeContinue(ch, ps) && ch != '<',
                            { textLen++; })

            // If we encountered '  ' we want to subtract 1 from the
            // length.
            if (ps.idx > 0 && flo_html_getChar(ps.text, ps.idx - 1) == ' ') {
                textLen--;
            }

            if (isAppend) {
                flo_html_addRawData(&raw, whiteSpace, perm);
            }
            isAppend = true;

            flo_html_addRawData(
                &raw,
                FLO_HTML_S_LEN(flo_html_getCharPtr(ps.text, textStart),
                               textLen),
                perm);
        }
        SKIP_EMPTY_SPACE(ps);
    })

    dom->nodes.buf[nodeID].text = FLO_HTML_S_LEN(raw.buf, raw.len);

    return (flo_html_TextNodeParseResult){.newNodeID = nodeID,
                                          .nextIndex = ps.idx};
}

flo_html_String parseProp(ParseStatus *ps) {
    ptrdiff_t propStart = ps->idx;

    unsigned ch = currentChar(*ps);
    ptrdiff_t propLen;
    if (ch == '\'' || ch == '"') {
        unsigned char quote = ch;

        ptrdiff_t quoteCount = 0;
        propLen = -2; // 2 quotes so we start at -2.
        NEXT_CHAR_WHILE(*ps, quoteCount < 2, {
            quoteCount += (ch == quote);
            propLen++;
        })
        propStart++;
    } else {
        propLen = 0;
        NEXT_CHAR_UNTIL(*ps,
                        ch == ' ' || flo_html_isSpecialSpace(ch) || ch == '=' ||
                            ch == '>',
                        { propLen++; })
    }
    FLO_HTML_ASSERT(propLen >= 0);

    return FLO_HTML_S_LEN(flo_html_getCharPtr(ps->text, propStart), propLen);
}

flo_html_NodeParseResult parseDocumentNode(ParseStatus ps, bool exclamTag,
                                           flo_html_Dom *dom,
                                           flo_html_Arena *perm) {
    flo_html_NodeParseResult result;
    flo_html_node_id nodeID =
        flo_html_createNode(NODE_TYPE_DOCUMENT, dom, perm);

    ptrdiff_t tagStart = ps.idx;
    NEXT_CHAR_UNTIL(ps, ch == '>' || ch == ' ' || flo_html_isSpecialSpace(ch),
                    {});
    ptrdiff_t tagSize = ps.idx - tagStart;

    bool canHaveChildren = !exclamTag;

    // For example <input/>.
    if (currentChar(ps) == '>' &&
        flo_html_getChar(ps.text, ps.idx - 1) == '/') {
        canHaveChildren = false;
        tagSize--;
    }

    flo_html_String documentTag =
        FLO_HTML_S_LEN(flo_html_getCharPtr(ps.text, tagStart), tagSize);
    // For example <br>.
    if (isVoidElement(documentTag)) {
        canHaveChildren = false;
    }

    SKIP_EMPTY_SPACE(ps);

    PARSE_CHAR_UNTIL(ps, ch == '>', {
        if (ch == '/') {
            canHaveChildren = false;
            ps.idx++;
        } else if (ch != ' ' && !flo_html_isSpecialSpace(ch)) {
            // Found a property.
            // Not sure yet if it is a boolean or key-value property.
            // Accepted values:
            // 'property' / "property" / property
            flo_html_String propKey = parseProp(&ps);
            // Check for key-value property
            if (currentChar(ps) == '=') {
                // Expected syntax: key="value" OR
                // Expected syntax: key='value' OR
                // Expected syntax: key=value (This is invalid html, but
                // will still support it) We can do some more interesting
                // stuff but currently not required.
                NEXT_CHAR(ps); // skip '='
                flo_html_String propValue = parseProp(&ps);
                flo_html_addPropertyToNode(nodeID, propKey, propValue, dom,
                                           perm);
            } else {
                flo_html_addBooleanPropertyToNode(nodeID, propKey, dom, perm);
            }
        }
        SKIP_EMPTY_SPACE(ps);
    })

    flo_html_setTagOnDocumentNode(documentTag, nodeID, canHaveChildren, dom,
                                  perm);

    NEXT_CHAR(ps); // Skip '>', we basically already parsed it by adding
                   // the tag to the node ID.

    result.nodeID = nodeID;
    result.canHaveChildren = canHaveChildren;
    result.tag = documentTag;
    result.nextPosition = ps.idx;

    return result;
}

void flo_html_parse(flo_html_String html, flo_html_Dom *dom,
                    flo_html_node_id prevNodeID, flo_html_NodeDepth *nodeStack,
                    flo_html_Arena *perm) {
    ParseStatus ps = {.idx = 0, .text = html};
    ptrdiff_t nodeStartLen = nodeStack->len;

    SKIP_EMPTY_SPACE(ps);

    while (ps.idx < ps.text.len) {
        // Open document node.
        if (currentChar(ps) == '<' &&
            ps.idx < ps.text.len - 1) { // TODO: Can we remove this  ps.idx
                                        // < ps.text.len - 1
            if (isCommentTag(ps)) {
                SKIP_COMMENT(ps)
                // TODO: what to do with parseResult updateR??
            } else if (isEndDocumentTag(
                           ps, nodeStack->stack[nodeStack->len - 1].tag)) {
                if (nodeStack->len > nodeStartLen) {
                    nodeStack->len--;
                    prevNodeID = nodeStack->stack[nodeStack->len].nodeID;
                }
                NEXT_CHAR_UNTIL(ps, (ch == '>'), {});
                NEXT_CHAR(ps);
                // TODO: what to do with parseResult updateR??
            } else {
                NEXT_CHAR(ps);
                unsigned char ch = currentChar(ps);

                if (flo_html_isAlphaBetical(ch) || ch == '!') {
                    // standard opening tag or !DOCTYPE and friends
                    flo_html_NodeParseResult parseResult =
                        parseDocumentNode(ps, ch == '!', dom, perm);

                    updateReferences(parseResult.nodeID, prevNodeID, nodeStack,
                                     dom, perm);

                    if (parseResult.canHaveChildren) {
                        if (nodeStack->len >= FLO_HTML_MAX_NODE_DEPTH) {
                            FLO_HTML_PRINT_ERROR("Reached max node depth\n");
                        }
                        nodeStack->stack[nodeStack->len].nodeID =
                            parseResult.nodeID;
                        nodeStack->stack[nodeStack->len].tag = parseResult.tag;
                        nodeStack->len++;
                    }
                    prevNodeID = parseResult.nodeID;
                    ps.idx = parseResult.nextPosition;
                    // TODO: what to do with updated ps ????
                } else {
                    // Rogue open tag -> Text node.
                    flo_html_TextNodeParseResult parseResult =
                        parseTextNode(ps, dom, perm);
                    updateReferences(parseResult.newNodeID, prevNodeID,
                                     nodeStack, dom, perm);
                    prevNodeID = parseResult.newNodeID;
                    ps.idx = parseResult.nextIndex;
                    // TODO: what to do with parseResult updateR??
                }
            }
        }
        // Text node.
        else {
            flo_html_TextNodeParseResult parseResult =
                parseTextNode(ps, dom, perm);
            updateReferences(parseResult.newNodeID, prevNodeID, nodeStack, dom,
                             perm);
            prevNodeID = parseResult.newNodeID;
            ps.idx = parseResult.nextIndex;
            // TODO: what to do with parseResult updateR??
        }

        SKIP_EMPTY_SPACE(ps);
    }
}

void flo_html_parseRoot(flo_html_String html, flo_html_Dom *dom,
                        flo_html_Arena *perm) {
    flo_html_NodeDepth nodeStack;
    nodeStack.stack[0].nodeID = FLO_HTML_ROOT_NODE_ID;
    nodeStack.stack[0].tag = FLO_HTML_EMPTY_STRING;
    nodeStack.len = 1;

    flo_html_parse(html, dom, FLO_HTML_ROOT_NODE_ID, &nodeStack, perm);
}

void flo_html_parseExtra(flo_html_String html, flo_html_Dom *dom,
                         flo_html_Arena *perm) {
    flo_html_NodeDepth nodeStack;
    nodeStack.len = 0;

    flo_html_parse(html, dom, FLO_HTML_ERROR_NODE_ID, &nodeStack, perm);
}

flo_html_node_id
flo_html_parseDocumentElement(flo_html_DocumentNode *documentNode,
                              flo_html_Dom *dom, flo_html_Arena *perm) {
    flo_html_node_id newNodeID =
        flo_html_createNode(NODE_TYPE_DOCUMENT, dom, perm);

    flo_html_setTagOnDocumentNode(documentNode->tag, newNodeID,
                                  documentNode->isPaired, dom, perm);

    for (ptrdiff_t i = 0; i < documentNode->boolPropsLen; i++) {
        flo_html_String boolProp = documentNode->boolProps[i];
        flo_html_addBooleanPropertyToNode(newNodeID, boolProp, dom, perm);
    }

    for (ptrdiff_t i = 0; i < documentNode->propsLen; i++) {
        flo_html_String keyProp = documentNode->keyProps[i];
        flo_html_String valueProp = documentNode->valueProps[i];
        flo_html_addPropertyToNode(newNodeID, keyProp, valueProp, dom, perm);
    }

    return newNodeID;
}

flo_html_node_id flo_html_parseTextElement(flo_html_String text,
                                           flo_html_Dom *dom,
                                           flo_html_Arena *perm) {
    flo_html_node_id newNodeID = flo_html_createNode(NODE_TYPE_TEXT, dom, perm);
    unsigned char *malloced = FLO_HTML_NEW(perm, unsigned char, text.len);
    memcpy(malloced, text.buf, text.len);
    dom->nodes.buf[newNodeID].text = FLO_HTML_S_LEN(malloced, text.len);

    return newNodeID;
}
