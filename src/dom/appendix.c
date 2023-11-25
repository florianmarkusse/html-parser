
#include <string.h>

#include "error.h"
#include "file/read.h"
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
#include "log.h"

#define APPEND_USING_QUERYSELECTOR(cssQuery, nodeData, parsed, perm,           \
                                   appendFunction)                             \
    do {                                                                       \
        flo_html_node_id parentNodeID = 0;                                     \
        flo_html_QueryStatus queryResult =                                     \
            flo_html_querySelector(cssQuery, parsed, &parentNodeID, *(perm));  \
        if (queryResult != QUERY_SUCCESS) {                                    \
            FLO_FLUSH_AFTER(FLO_STDERR) {                                      \
                FLO_ERROR("Could not find element using query selector: ");    \
                FLO_ERROR((cssQuery), FLO_NEWLINE);                            \
            }                                                                  \
            return 0;                                                          \
        }                                                                      \
        return appendFunction(parentNodeID, nodeData, parsed, perm);           \
    } while (0)

flo_html_node_id
flo_html_appendDocumentNodeWithQuery(flo_String cssQuery,
                                     flo_html_DocumentNode *docNode,
                                     flo_html_Dom *dom, flo_Arena *perm) {
    APPEND_USING_QUERYSELECTOR(cssQuery, docNode, dom, perm,
                               flo_html_appendDocumentNode);
}

flo_html_node_id flo_html_appendTextNodeWithQuery(flo_String cssQuery,
                                                  flo_String text,
                                                  flo_html_Dom *dom,
                                                  flo_Arena *perm) {
    APPEND_USING_QUERYSELECTOR(cssQuery, text, dom, perm,
                               flo_html_appendTextNode);
}

flo_html_node_id flo_html_appendHTMLFromStringWithQuery(flo_String cssQuery,
                                                        flo_String htmlString,
                                                        flo_html_Dom *dom,
                                                        flo_Arena *perm) {
    APPEND_USING_QUERYSELECTOR(cssQuery, htmlString, dom, perm,
                               flo_html_appendHTMLFromString);
}

flo_html_node_id flo_html_appendHTMLFromFileWithQuery(flo_String cssQuery,
                                                      char *fileLocation,
                                                      flo_html_Dom *dom,
                                                      flo_Arena *perm) {
    flo_String content;
    flo_FileStatus fileStatus = flo_readFile(fileLocation, &content, perm);
    if (fileStatus != FILE_SUCCESS) {
        FLO_FLUSH_AFTER(FLO_STDERR) {
            FLO_ERROR(flo_fileStatusToString(fileStatus), FLO_NEWLINE);
            FLO_ERROR("Failed to read file: ");
            FLO_ERROR(fileLocation, FLO_NEWLINE);
        }
        return FLO_HTML_ERROR_NODE_ID;
    }

    APPEND_USING_QUERYSELECTOR(cssQuery, content, dom, perm,
                               flo_html_appendHTMLFromString);
}

static void updateReferences(flo_html_node_id parentID,
                             flo_html_node_id newNodeID, flo_html_Dom *dom,
                             flo_Arena *perm) {
    flo_html_ParentChild *firstChild =
        flo_html_getFirstChildNode(parentID, dom);
    if (firstChild == NULL) {
        *FLO_PUSH(&dom->parentFirstChilds, perm) =
            (flo_html_ParentChild){.parentID = parentID, .childID = newNodeID};
        *FLO_PUSH(&dom->parentChilds, perm) =
            (flo_html_ParentChild){.parentID = parentID, .childID = newNodeID};

        flo_html_connectOtherNodesToParent(parentID, newNodeID, dom, perm);
        return;
    }

    flo_html_node_id lastNextNode =
        flo_html_getLastNext(firstChild->childID, dom);

    *FLO_PUSH(&dom->nextNodes, perm) = (flo_html_NextNode){
        .currentNodeID = lastNextNode, .nextNodeID = newNodeID};
    *FLO_PUSH(&dom->parentChilds, perm) =
        (flo_html_ParentChild){.parentID = parentID, .childID = newNodeID};

    flo_html_connectOtherNodesToParent(parentID, newNodeID, dom, perm);
}

flo_html_node_id flo_html_appendDocumentNode(flo_html_node_id parentID,
                                             flo_html_DocumentNode *docNode,
                                             flo_html_Dom *dom,
                                             flo_Arena *perm) {
    flo_html_node_id newNodeID =
        flo_html_parseDocumentElement(docNode, dom, perm);
    if (newNodeID == 0) {
        return 0;
    }
    updateReferences(parentID, newNodeID, dom, perm);
    return newNodeID;
}

flo_html_node_id flo_html_appendTextNode(flo_html_node_id parentID,
                                         flo_String text, flo_html_Dom *dom,
                                         flo_Arena *perm) {
    flo_html_node_id newNodeID = flo_html_parseTextElement(text, dom, perm);
    if (newNodeID == 0) {
        return 0;
    }

    flo_html_node_id child = flo_html_getFirstChild(parentID, dom);
    if (child > 0) {
        child = flo_html_getLastNext(child, dom);
        if (flo_html_tryMerge(child, newNodeID, dom, true, perm)) {
            flo_html_removeNode(newNodeID, dom);
            return child;
        }
    }

    updateReferences(parentID, newNodeID, dom, perm);
    return newNodeID;
}

flo_html_node_id flo_html_appendHTMLFromString(flo_html_node_id parentID,
                                               flo_String htmlString,
                                               flo_html_Dom *dom,
                                               flo_Arena *perm) {
    flo_html_node_id firstNewAddedNodeID = (flo_html_node_id)dom->nodes.len;
    dom = flo_html_parseExtra(htmlString, dom, perm);
    if (dom == NULL) {
        return 0;
    }

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
                if (flo_html_tryMerge(lastNext, firstAddedNode->nodeID, dom,
                                      true, perm)) {
                    flo_html_node_id secondNewAddedNode =
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
                if (flo_html_tryMerge(lastNext, firstAddedNode->nodeID, dom,
                                      true, perm)) {
                    flo_html_removeNode(firstNewAddedNodeID, dom);
                    return lastNext;
                }
            }
        }
    }

    updateReferences(parentID, firstNewAddedNodeID, dom, perm);

    return firstNewAddedNodeID;
}
