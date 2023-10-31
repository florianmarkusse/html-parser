#include <string.h>

#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/prependix.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/node/node.h"
#include "flo/html-parser/node/parent-child.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/file/read.h"

#define PREPEND_USING_QUERYSELECTOR(cssQuery, nodeData, dom, perm,             \
                                    prependFunction)                           \
    do {                                                                       \
        flo_html_node_id parentNodeID = 0;                                     \
        flo_html_QueryStatus queryResult =                                     \
            flo_html_querySelector(cssQuery, dom, &parentNodeID, *(perm));     \
        if (queryResult != QUERY_SUCCESS) {                                    \
            FLO_HTML_PRINT_ERROR(                                              \
                "Could not find element using query selector: %s\n",           \
                (cssQuery).buf);                                               \
            return 0;                                                          \
        }                                                                      \
        return prependFunction(parentNodeID, nodeData, dom, perm);             \
    } while (0)

flo_html_node_id
flo_html_prependDocumentNodeWithQuery(flo_html_String cssQuery,
                                      flo_html_DocumentNode *docNode,
                                      flo_html_Dom *dom, flo_html_Arena *perm) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, docNode, dom, perm,
                                flo_html_prependDocumentNode);
}

flo_html_node_id flo_html_prependTextNodeWithQuery(flo_html_String cssQuery,
                                                   flo_html_String text,
                                                   flo_html_Dom *dom,
                                                   flo_html_Arena *perm) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, text, dom, perm,
                                flo_html_prependTextNode);
}

flo_html_node_id flo_html_prependHTMLFromStringWithQuery(
    flo_html_String cssQuery, flo_html_String htmlString, flo_html_Dom *dom,
    flo_html_Arena *perm) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, htmlString, dom, perm,
                                flo_html_prependHTMLFromString);
}

flo_html_node_id
flo_html_prependHTMLFromFileWithQuery(flo_html_String cssQuery,
                                      flo_html_String fileLocation,
                                      flo_html_Dom *dom, flo_html_Arena *perm) {
    flo_html_String content;
    flo_html_FileStatus fileStatus =
        flo_html_readFile(fileLocation, &content, perm);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return FLO_HTML_ERROR_NODE_ID;
    }

    PREPEND_USING_QUERYSELECTOR(cssQuery, content, dom, perm,
                                flo_html_prependHTMLFromString);
}

static void updateReferences(flo_html_node_id parentID,
                             flo_html_node_id firstNewNodeID, flo_html_Dom *dom,
                             flo_html_Arena *perm) {
    flo_html_ParentChild *firstChild =
        flo_html_getFirstChildNode(parentID, dom);
    if (firstChild == NULL) {
        *FLO_HTML_PUSH(&dom->parentFirstChilds, perm) = (flo_html_ParentChild){
            .parentID = parentID, .childID = firstNewNodeID};
        *FLO_HTML_PUSH(&dom->parentChilds, perm) = (flo_html_ParentChild){
            .parentID = parentID, .childID = firstNewNodeID};

        flo_html_connectOtherNodesToParent(parentID, firstNewNodeID, dom, perm);

        return;
    }

    flo_html_node_id previousFirstChild = firstChild->childID;
    firstChild->childID = firstNewNodeID;

    flo_html_node_id lastNextOfNew = flo_html_getLastNext(firstNewNodeID, dom);

    *FLO_HTML_PUSH(&dom->nextNodes, perm) = (flo_html_NextNode){
        .currentNodeID = lastNextOfNew, .nextNodeID = previousFirstChild};
    *FLO_HTML_PUSH(&dom->parentChilds, perm) =
        (flo_html_ParentChild){.parentID = parentID, .childID = firstNewNodeID};

    flo_html_connectOtherNodesToParent(parentID, firstNewNodeID, dom, perm);
}

flo_html_node_id flo_html_prependDocumentNode(flo_html_node_id parentID,
                                              flo_html_DocumentNode *docNode,
                                              flo_html_Dom *dom,
                                              flo_html_Arena *perm) {
    flo_html_node_id newNodeID =
        flo_html_parseDocumentElement(docNode, dom, perm);
    if (newNodeID == 0) {
        return 0;
    }
    updateReferences(parentID, newNodeID, dom, perm);
    return newNodeID;
}

flo_html_node_id flo_html_prependTextNode(flo_html_node_id parentID,
                                          flo_html_String text,
                                          flo_html_Dom *dom,
                                          flo_html_Arena *perm) {
    flo_html_node_id newNodeID = flo_html_parseTextElement(text, dom, perm);
    if (newNodeID == 0) {
        return 0;
    }

    flo_html_node_id child = flo_html_getFirstChild(parentID, dom);
    if (child > 0) {
        if (flo_html_tryMerge(child, newNodeID, dom, false, perm)) {
            flo_html_removeNode(newNodeID, dom);
            return newNodeID;
        }
    }

    updateReferences(parentID, newNodeID, dom, perm);

    return newNodeID;
}

flo_html_node_id flo_html_prependHTMLFromString(flo_html_node_id parentID,
                                                flo_html_String htmlString,
                                                flo_html_Dom *dom,
                                                flo_html_Arena *perm) {
    flo_html_node_id firstNewAddedNode = (flo_html_node_id)dom->nodes.len;
    dom = flo_html_parseExtra(htmlString, dom, perm);
    if (dom == NULL) {
        return 0;
    }

    flo_html_node_id firstChild = flo_html_getFirstChild(parentID, dom);
    if (firstChild > 0) {
        flo_html_node_id lastNextNode =
            flo_html_getLastNext(firstNewAddedNode, dom);
        if (lastNextNode > firstNewAddedNode) {
            flo_html_Node *lastAddedNode = &dom->nodes.buf[lastNextNode];
            if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
                if (flo_html_tryMerge(firstChild, lastAddedNode->nodeID, dom,
                                      false, perm)) {
                    flo_html_removeNode(lastNextNode, dom);
                }
            }
        } else {
            flo_html_Node *onlyAddedNode = &dom->nodes.buf[firstNewAddedNode];
            if (onlyAddedNode->nodeType == NODE_TYPE_TEXT) {
                if (flo_html_tryMerge(firstChild, onlyAddedNode->nodeID, dom,
                                      false, perm)) {
                    flo_html_removeNode(firstNewAddedNode, dom);
                    return firstChild;
                }
            }
        }
    }

    updateReferences(parentID, firstNewAddedNode, dom, perm);

    return firstNewAddedNode;
}
