
#include <string.h>

#include "flo/html-parser/dom/appendix.h"
#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/node/parent-child.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/file/read.h"

#define APPEND_USING_QUERYSELECTOR(cssQuery, nodeData, parsed, perm,           \
                                   appendFunction)                             \
    do {                                                                       \
        flo_html_node_id parentNodeID = 0;                                     \
        flo_html_QueryStatus queryResult =                                     \
            flo_html_querySelector(cssQuery, parsed, &parentNodeID, *(perm));  \
        if (queryResult != QUERY_SUCCESS) {                                    \
            FLO_HTML_PRINT_ERROR(                                              \
                "Could not find element using query selector: %s\n",           \
                (cssQuery).buf);                                               \
            return 0;                                                          \
        }                                                                      \
        return appendFunction(parentNodeID, nodeData, parsed, perm);           \
    } while (0)

flo_html_node_id flo_html_appendDocumentNodeWithQuery(
    const flo_html_String cssQuery, const flo_html_DocumentNode *docNode,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    APPEND_USING_QUERYSELECTOR(cssQuery, docNode, parsed, perm,
                               flo_html_appendDocumentNode);
}

flo_html_node_id flo_html_appendTextNodeWithQuery(
    const flo_html_String cssQuery, const flo_html_String text,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    APPEND_USING_QUERYSELECTOR(cssQuery, text, parsed, perm,
                               flo_html_appendTextNode);
}

flo_html_node_id flo_html_appendHTMLFromStringWithQuery(
    const flo_html_String cssQuery, const flo_html_String htmlString,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    APPEND_USING_QUERYSELECTOR(cssQuery, htmlString, parsed, perm,
                               flo_html_appendHTMLFromString);
}

flo_html_node_id flo_html_appendHTMLFromFileWithQuery(
    const flo_html_String cssQuery, const flo_html_String fileLocation,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    flo_html_String content;
    flo_html_FileStatus fileStatus =
        flo_html_readFile(fileLocation, &content, perm);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return FLO_HTML_ERROR_NODE_ID;
    }

    APPEND_USING_QUERYSELECTOR(cssQuery, content, parsed, perm,
                               flo_html_appendHTMLFromString);
}

static void updateReferences(const flo_html_node_id parentID,
                             const flo_html_node_id newNodeID,
                             flo_html_Dom *dom, flo_html_Arena *perm) {
    flo_html_ParentChild *firstChild =
        flo_html_getFirstChildNode(parentID, dom);
    if (firstChild == NULL) {
        flo_html_addParentFirstChild(parentID, newNodeID, dom, perm);
        flo_html_addParentChild(parentID, newNodeID, dom, perm);
        flo_html_connectOtherNodesToParent(parentID, newNodeID, dom, perm);
        return;
    }

    flo_html_node_id lastNextNode =
        flo_html_getLastNext(firstChild->childID, dom);

    flo_html_addNextNode(lastNextNode, newNodeID, dom, perm);
    flo_html_addParentChild(parentID, newNodeID, dom, perm);
    flo_html_connectOtherNodesToParent(parentID, newNodeID, dom, perm);
}

flo_html_node_id
flo_html_appendDocumentNode(const flo_html_node_id parentID,
                            const flo_html_DocumentNode *docNode,
                            flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    flo_html_node_id newNodeID =
        flo_html_parseDocumentElement(docNode, parsed, perm);
    updateReferences(parentID, newNodeID, parsed.dom, perm);
    return newNodeID;
}

flo_html_node_id flo_html_appendTextNode(const flo_html_node_id parentID,
                                         const flo_html_String text,
                                         flo_html_ParsedHTML parsed,
                                         flo_html_Arena *perm) {
    flo_html_Dom *dom = parsed.dom;

    flo_html_node_id newNodeID = flo_html_parseTextElement(text, parsed, perm);

    flo_html_node_id child = flo_html_getFirstChild(parentID, dom);
    if (child > 0) {
        child = flo_html_getLastNext(child, dom);
        if (flo_html_tryMerge(&dom->nodes.buf[child],
                              &dom->nodes.buf[newNodeID], parsed, true, perm)) {
            flo_html_removeNode(newNodeID, dom);
            return child;
        }
    }

    updateReferences(parentID, newNodeID, dom, perm);
    return newNodeID;
}

flo_html_node_id flo_html_appendHTMLFromString(const flo_html_node_id parentID,
                                               const flo_html_String htmlString,
                                               flo_html_ParsedHTML parsed,
                                               flo_html_Arena *perm) {
    flo_html_Dom *dom = parsed.dom;

    flo_html_node_id firstNewAddedNodeID = dom->nodes.len;
    flo_html_parseExtra(htmlString, parsed, perm);

    flo_html_node_id firstChild = flo_html_getFirstChild(parentID, dom);
    if (firstChild > 0) {
        flo_html_node_id lastNewAddedNodeID =
            flo_html_getLastNext(firstNewAddedNodeID, dom);
        if (lastNewAddedNodeID > firstNewAddedNodeID) {
            flo_html_Node *firstAddedNode =
                &dom->nodes.buf[firstNewAddedNodeID];
            if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
                flo_html_node_id lastNext =
                    flo_html_getLastNext(firstChild, dom);
                if (flo_html_tryMerge(&dom->nodes.buf[lastNext], firstAddedNode,
                                      parsed, true, perm)) {
                    ptrdiff_t secondNewAddedNode =
                        flo_html_getNext(firstNewAddedNodeID, dom);
                    flo_html_removeNode(firstNewAddedNodeID, dom);
                    firstNewAddedNodeID = secondNewAddedNode;
                }
            }
        } else {
            flo_html_Node *firstAddedNode =
                &dom->nodes.buf[firstNewAddedNodeID];
            if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
                flo_html_node_id lastNext =
                    flo_html_getLastNext(firstChild, dom);
                if (flo_html_tryMerge(&dom->nodes.buf[lastNext], firstAddedNode,
                                      parsed, true, perm)) {
                    flo_html_removeNode(firstNewAddedNodeID, dom);
                    return lastNext;
                }
            }
        }
    }

    updateReferences(parentID, firstNewAddedNodeID, dom, perm);

    return firstNewAddedNodeID;
}
