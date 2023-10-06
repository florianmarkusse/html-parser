#include <string.h>

#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/prependix/prependix.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/file/read.h"
#include "flo/html-parser/utils/print/error.h"

#define PREPEND_USING_QUERYSELECTOR(cssQuery, nodeData, dom, textStore,        \
                                    prependFunction)                           \
    do {                                                                       \
        flo_html_node_id parentNodeID = 0;                                     \
        flo_html_QueryStatus queryResult =                                     \
            flo_html_querySelector(cssQuery, dom, textStore, &parentNodeID);   \
        if (queryResult != QUERY_SUCCESS) {                                    \
            FLO_HTML_PRINT_ERROR(                                              \
                "Could not find element using query selector: %s\n",           \
                (cssQuery).buf);                                               \
            return DOM_NO_ELEMENT;                                             \
        }                                                                      \
        return prependFunction(parentNodeID, nodeData, dom, textStore);        \
    } while (0)

flo_html_DomStatus flo_html_prependDocumentNodeWithQuery(
    const flo_html_String cssQuery, const flo_html_DocumentNode *docNode,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, docNode, dom, textStore,
                                flo_html_prependDocumentNode);
}

flo_html_DomStatus
flo_html_prependTextNodeWithQuery(const flo_html_String cssQuery,
                                  const flo_html_String text, flo_html_Dom *dom,
                                  flo_html_TextStore *textStore) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, text, dom, textStore,
                                flo_html_prependTextNode);
}

flo_html_DomStatus flo_html_prependHTMLFromStringWithQuery(
    const flo_html_String cssQuery, const flo_html_String htmlString,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    PREPEND_USING_QUERYSELECTOR(cssQuery, htmlString, dom, textStore,
                                flo_html_prependHTMLFromString);
}

flo_html_DomStatus flo_html_prependHTMLFromFileWithQuery(
    const flo_html_String cssQuery, const flo_html_String fileLocation,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_String content;
    flo_html_FileStatus fileStatus = flo_html_readFile(fileLocation, &content);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation.buf);
        return DOM_ERROR_MEMORY;
    }

    PREPEND_USING_QUERYSELECTOR(cssQuery, content, dom, textStore,
                                flo_html_prependHTMLFromString);
}

static flo_html_DomStatus
updateReferences(const flo_html_node_id parentID,
                 const flo_html_node_id firstNewNodeID, flo_html_Dom *dom) {
    flo_html_DomStatus domStatus = DOM_SUCCESS;
    if (parentID == 0) {
        flo_html_node_id previousFirstNodeID = dom->firstNodeID;
        domStatus = flo_html_addNextNode(firstNewNodeID, dom->firstNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add new node ID in next nodes!\n");
            return domStatus;
        }
        dom->firstNodeID = firstNewNodeID;

        flo_html_node_id lastNextNode =
            flo_html_getLastNext(dom->firstNodeID, dom);
        if (lastNextNode > firstNewNodeID) {
            domStatus =
                flo_html_addNextNode(lastNextNode, previousFirstNodeID, dom);
            if (domStatus != DOM_SUCCESS) {
                FLO_HTML_PRINT_ERROR(
                    "Failed to add new node ID in next nodes!\n");
                return domStatus;
            }
        }

        return domStatus;
    }

    flo_html_ParentChild *firstChild =
        flo_html_getFirstChildNode(parentID, dom);
    if (firstChild == NULL) {
        domStatus = flo_html_addParentFirstChild(parentID, firstNewNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add new node ID as first child!\n");
            return domStatus;
        }

        domStatus = flo_html_addParentChild(parentID, firstNewNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add new node ID as child!\n");
            return domStatus;
        }

        domStatus =
            flo_html_connectOtherNodesToParent(parentID, firstNewNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add remaning node IDs as child!\n");
            return domStatus;
        }

        return domStatus;
    }

    flo_html_node_id previousFirstChild = firstChild->childID;
    firstChild->childID = firstNewNodeID;

    flo_html_node_id lastNextOfNew = flo_html_getLastNext(firstNewNodeID, dom);
    domStatus = flo_html_addNextNode(lastNextOfNew, previousFirstChild, dom);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add new node ID in next nodes!\n");
        return domStatus;
    }

    domStatus = flo_html_addParentChild(parentID, firstNewNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add new node ID as child!\n");
        return domStatus;
    }

    domStatus =
        flo_html_connectOtherNodesToParent(parentID, firstNewNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add remaning node IDs as child!\n");
        return domStatus;
    }

    return domStatus;
}

flo_html_DomStatus
flo_html_prependDocumentNode(const flo_html_node_id parentID,
                             const flo_html_DocumentNode *docNode,
                             flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_node_id newNodeID = 0;
    flo_html_DomStatus domStatus =
        flo_html_parseDocumentElement(docNode, dom, textStore, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse document element!\n");
        return domStatus;
    }
    return updateReferences(parentID, newNodeID, dom);
}

flo_html_DomStatus flo_html_prependTextNode(const flo_html_node_id parentID,
                                            const flo_html_String text,
                                            flo_html_Dom *dom,
                                            flo_html_TextStore *textStore) {
    flo_html_node_id newNodeID = 0;
    flo_html_DomStatus domStatus =
        flo_html_parseTextElement(text, dom, textStore, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse text element!\n");
        return domStatus;
    }

    flo_html_node_id child = flo_html_getFirstChild(parentID, dom);
    if (child > 0) {
        flo_html_MergeResult mergeTry = flo_html_tryMerge(
            &dom->nodes[child], &dom->nodes[newNodeID], dom, textStore, false);

        if (mergeTry == COMPLETED_MERGE) {
            flo_html_removeNode(newNodeID, dom);
            return domStatus;
        }

        if (mergeTry == FAILED_MERGE) {
            return DOM_NO_ADD;
        }
    }

    return updateReferences(parentID, newNodeID, dom);
}

flo_html_DomStatus flo_html_prependHTMLFromString(
    const flo_html_node_id parentID, const flo_html_String htmlString,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_node_id firstNewAddedNode = dom->nodeLen;
    flo_html_DomStatus domStatus =
        flo_html_parseExtra(htmlString, dom, textStore);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse string!\n");
        return domStatus;
    }

    flo_html_node_id firstChild = flo_html_getFirstChild(parentID, dom);
    if (firstChild > 0) {
        flo_html_node_id lastNextNode =
            flo_html_getLastNext(firstNewAddedNode, dom);
        if (lastNextNode > firstNewAddedNode) {
            flo_html_Node *lastAddedNode = &dom->nodes[lastNextNode];
            if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
                flo_html_MergeResult mergeResult =
                    flo_html_tryMerge(&dom->nodes[firstChild], lastAddedNode,
                                      dom, textStore, false);
                if (mergeResult == COMPLETED_MERGE) {
                    flo_html_removeNode(lastNextNode, dom);
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        } else {
            flo_html_Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
            if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
                flo_html_MergeResult mergeResult =
                    flo_html_tryMerge(&dom->nodes[firstChild], firstAddedNode,
                                      dom, textStore, false);
                if (mergeResult == COMPLETED_MERGE) {
                    flo_html_removeNode(firstNewAddedNode, dom);
                    return domStatus;
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        }
    }

    return updateReferences(parentID, firstNewAddedNode, dom);
}
