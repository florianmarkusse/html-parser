
#include <string.h>

#include "flo/html-parser/dom/appendix/appendix.h"
#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/file/read.h"
#include "flo/html-parser/utils/print/error.h"

#define APPEND_USING_QUERYSELECTOR(cssQuery, nodeData, dom, textStore,         \
                                   appendFunction)                             \
    do {                                                                       \
        flo_html_node_id parentNodeID = 0;                                     \
        flo_html_QueryStatus queryResult =                                     \
            flo_html_querySelector(cssQuery, dom, textStore, &parentNodeID);   \
        if (queryResult != QUERY_SUCCESS) {                                    \
            FLO_HTML_PRINT_ERROR(                                              \
                "Could not find element using query selector: %s\n",           \
                cssQuery);                                                     \
            return DOM_NO_ELEMENT;                                             \
        }                                                                      \
        return appendFunction(parentNodeID, nodeData, dom, textStore);         \
    } while (0)

flo_html_DomStatus flo_html_appendDocumentNodeWithQuery(
    const char *cssQuery, const flo_html_DocumentNode *docNode,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    APPEND_USING_QUERYSELECTOR(cssQuery, docNode, dom, textStore,
                               flo_html_appendDocumentNode);
}

flo_html_DomStatus
flo_html_appendTextNodeWithQuery(const char *cssQuery, const char *text,
                                 flo_html_Dom *dom,
                                 flo_html_TextStore *textStore) {
    APPEND_USING_QUERYSELECTOR(cssQuery, text, dom, textStore,
                               flo_html_appendTextNode);
}

flo_html_DomStatus flo_html_appendHTMLFromStringWithQuery(
    const char *cssQuery, const char *htmlString, flo_html_Dom *dom,
    flo_html_TextStore *textStore) {
    APPEND_USING_QUERYSELECTOR(cssQuery, htmlString, dom, textStore,
                               flo_html_appendHTMLFromString);
}

flo_html_DomStatus flo_html_appendHTMLFromFileWithQuery(
    const char *cssQuery, const char *fileLocation, flo_html_Dom *dom,
    flo_html_TextStore *textStore) {
    char *buffer = NULL;
    flo_html_FileStatus fileStatus = flo_html_readFile(fileLocation, &buffer);
    if (fileStatus != FILE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_fileStatusToString(fileStatus),
                                        "Failed to read file: \"%s\"",
                                        fileLocation);
        return DOM_ERROR_MEMORY;
    }

    APPEND_USING_QUERYSELECTOR(cssQuery, buffer, dom, textStore,
                               flo_html_appendHTMLFromString);
}

static flo_html_DomStatus updateReferences(const flo_html_node_id parentID,
                                           const flo_html_node_id newNodeID,
                                           flo_html_Dom *dom) {
    flo_html_DomStatus domStatus = DOM_SUCCESS;
    if (parentID == 0) {
        flo_html_node_id lastNextNode =
            flo_html_getLastNext(dom->firstNodeID, dom);

        domStatus = flo_html_addNextNode(lastNextNode, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add new node ID in next nodes!\n");
            return domStatus;
        }
    }

    flo_html_ParentChild *firstChild =
        flo_html_getFirstChildNode(parentID, dom);
    if (firstChild == NULL) {
        domStatus = flo_html_addParentFirstChild(parentID, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add new node ID as first child!\n");
            return domStatus;
        }

        domStatus = flo_html_addParentChild(parentID, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add new node ID as child!\n");
            return domStatus;
        }

        domStatus =
            flo_html_connectOtherNodesToParent(parentID, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add remaning node IDs as child!\n");
            return domStatus;
        }

        return domStatus;
    }

    flo_html_node_id lastNextNode =
        flo_html_getLastNext(firstChild->childID, dom);

    domStatus = flo_html_addNextNode(lastNextNode, newNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add new node ID in next nodes!\n");
        return domStatus;
    }

    domStatus = flo_html_addParentChild(parentID, newNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add new node ID as child!\n");
        return domStatus;
    }

    domStatus = flo_html_connectOtherNodesToParent(parentID, newNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add remaning node IDs as child!\n");
        return domStatus;
    }

    return domStatus;
}

flo_html_DomStatus
flo_html_appendDocumentNode(const flo_html_node_id parentID,
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

flo_html_DomStatus flo_html_appendTextNode(const flo_html_node_id parentID,
                                           const char *text, flo_html_Dom *dom,
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
        child = flo_html_getLastNext(child, dom);

        flo_html_MergeResult mergeTry = flo_html_tryMerge(
            &dom->nodes[child], &dom->nodes[newNodeID], dom, textStore, true);

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

flo_html_DomStatus
flo_html_appendHTMLFromString(const flo_html_node_id parentID,
                              const char *htmlString, flo_html_Dom *dom,
                              flo_html_TextStore *textStore) {
    flo_html_node_id firstNewAddedNode = dom->nodeLen;
    flo_html_DomStatus domStatus = flo_html_parse(htmlString, dom, textStore);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse string!\n");
        return domStatus;
    }

    flo_html_node_id firstChild = flo_html_getFirstChild(parentID, dom);
    if (firstChild > 0) {
        flo_html_node_id lastNextNode =
            flo_html_getLastNext(firstNewAddedNode, dom);
        if (lastNextNode > firstNewAddedNode) {
            flo_html_Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
            if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
                flo_html_node_id lastNext =
                    flo_html_getLastNext(firstChild, dom);
                flo_html_MergeResult mergeResult =
                    flo_html_tryMerge(&dom->nodes[lastNext], firstAddedNode,
                                      dom, textStore, true);
                if (mergeResult == COMPLETED_MERGE) {
                    size_t secondNewAddedNode =
                        flo_html_getNext(firstNewAddedNode, dom);
                    flo_html_removeNode(firstNewAddedNode, dom);
                    firstNewAddedNode = secondNewAddedNode;
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        } else {
            flo_html_Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
            if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
                flo_html_node_id lastNext =
                    flo_html_getLastNext(firstChild, dom);
                flo_html_MergeResult mergeResult =
                    flo_html_tryMerge(&dom->nodes[lastNext], firstAddedNode,
                                      dom, textStore, true);
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
