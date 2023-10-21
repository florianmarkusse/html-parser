#include <string.h>

#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/prependix.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/node/node.h"
#include "flo/html-parser/node/parent-child.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/file/read.h"

#define PREPEND_USING_QUERYSELECTOR(cssQuery, nodeData, parsed, perm,          \
                                    prependFunction)                           \
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
        return prependFunction(parentNodeID, nodeData, parsed, perm);          \
    } while (0)

flo_html_node_id flo_html_prependDocumentNodeWithQuery(
    const flo_html_String cssQuery, const flo_html_DocumentNode *docNode,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, docNode, parsed, perm,
                                flo_html_prependDocumentNode);
}

flo_html_node_id flo_html_prependTextNodeWithQuery(
    const flo_html_String cssQuery, const flo_html_String text,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, text, parsed, perm,
                                flo_html_prependTextNode);
}

flo_html_node_id flo_html_prependHTMLFromStringWithQuery(
    const flo_html_String cssQuery, const flo_html_String htmlString,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, htmlString, parsed, perm,
                                flo_html_prependHTMLFromString);
}

flo_html_node_id flo_html_prependHTMLFromFileWithQuery(
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

    PREPEND_USING_QUERYSELECTOR(cssQuery, content, parsed, perm,
                                flo_html_prependHTMLFromString);
}

static void updateReferences(const flo_html_node_id parentID,
                             const flo_html_node_id firstNewNodeID,
                             flo_html_Dom *dom, flo_html_Arena *perm) {
    flo_html_ParentChild *firstChild =
        flo_html_getFirstChildNode(parentID, dom);
    if (firstChild == NULL) {
        flo_html_addParentFirstChild(parentID, firstNewNodeID, dom, perm);
        flo_html_addParentChild(parentID, firstNewNodeID, dom, perm);
        flo_html_connectOtherNodesToParent(parentID, firstNewNodeID, dom, perm);

        return;
    }

    flo_html_node_id previousFirstChild = firstChild->childID;
    firstChild->childID = firstNewNodeID;

    flo_html_node_id lastNextOfNew = flo_html_getLastNext(firstNewNodeID, dom);

    flo_html_addNextNode(lastNextOfNew, previousFirstChild, dom, perm);
    flo_html_addParentChild(parentID, firstNewNodeID, dom, perm);
    flo_html_connectOtherNodesToParent(parentID, firstNewNodeID, dom, perm);
}

flo_html_node_id
flo_html_prependDocumentNode(const flo_html_node_id parentID,
                             const flo_html_DocumentNode *docNode,
                             flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    flo_html_node_id newNodeID =
        flo_html_parseDocumentElement(docNode, parsed, perm);
    updateReferences(parentID, newNodeID, parsed.dom, perm);
    return newNodeID;
}

flo_html_node_id flo_html_prependTextNode(const flo_html_node_id parentID,
                                          const flo_html_String text,
                                          flo_html_ParsedHTML parsed,
                                          flo_html_Arena *perm) {
    flo_html_node_id newNodeID = flo_html_parseTextElement(text, parsed, perm);

    flo_html_node_id child = flo_html_getFirstChild(parentID, parsed.dom);
    if (child > 0) {
        if (flo_html_tryMerge(&parsed.dom->nodes.buf[child],
                              &parsed.dom->nodes.buf[newNodeID], parsed,
                              false)) {
            flo_html_removeNode(newNodeID, parsed.dom);
            return newNodeID;
        }
    }

    updateReferences(parentID, newNodeID, parsed.dom, perm);

    return newNodeID;
}

flo_html_node_id flo_html_prependHTMLFromString(
    const flo_html_node_id parentID, const flo_html_String htmlString,
    flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    flo_html_node_id firstNewAddedNode = parsed.dom->nodes.len;
    flo_html_parseExtra(htmlString, parsed, perm);

    flo_html_node_id firstChild = flo_html_getFirstChild(parentID, parsed.dom);
    if (firstChild > 0) {
        flo_html_node_id lastNextNode =
            flo_html_getLastNext(firstNewAddedNode, parsed.dom);
        if (lastNextNode > firstNewAddedNode) {
            flo_html_Node *lastAddedNode = &parsed.dom->nodes.buf[lastNextNode];
            if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
                if (flo_html_tryMerge(&parsed.dom->nodes.buf[firstChild],
                                      lastAddedNode, parsed, false)) {
                    flo_html_removeNode(lastNextNode, parsed.dom);
                }
            }
        } else {
            flo_html_Node *onlyAddedNode =
                &parsed.dom->nodes.buf[firstNewAddedNode];
            if (onlyAddedNode->nodeType == NODE_TYPE_TEXT) {
                if (flo_html_tryMerge(&parsed.dom->nodes.buf[firstChild],
                                      onlyAddedNode, parsed, false)) {
                    flo_html_removeNode(firstNewAddedNode, parsed.dom);
                    return firstChild;
                }
            }
        }
    }

    updateReferences(parentID, firstNewAddedNode, parsed.dom, perm);

    return firstNewAddedNode;
}
