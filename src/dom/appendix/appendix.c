
#include <string.h>

#include "flo/html-parser/dom/appendix/appendix.h"
#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/utils.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/print/error.h"

static DomStatus updateReferences(const node_id parentID,
                                  const node_id newNodeID, Dom *dom) {
    DomStatus domStatus = DOM_SUCCESS;
    if (parentID == 0) {
        node_id lastNextNode = getLastNext(dom->firstNodeID, dom);

        domStatus = addNextNode(lastNextNode, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID in next nodes!\n");
            return domStatus;
        }
    }

    ParentChild *firstChild = getFirstChildNode(parentID, dom);
    if (firstChild == NULL) {
        domStatus = addParentFirstChild(parentID, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID as first child!\n");
            return domStatus;
        }

        domStatus = addParentChild(parentID, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID as child!\n");
            return domStatus;
        }

        domStatus = connectOtherNodesToParent(parentID, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add remaning node IDs as child!\n");
            return domStatus;
        }

        return domStatus;
    }

    node_id lastNextNode = getLastNext(firstChild->childID, dom);

    domStatus = addNextNode(lastNextNode, newNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add new node ID in next nodes!\n");
        return domStatus;
    }

    domStatus = addParentChild(parentID, newNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add new node ID as child!\n");
        return domStatus;
    }

    domStatus = connectOtherNodesToParent(parentID, newNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add remaning node IDs as child!\n");
        return domStatus;
    }

    return domStatus;
}

DomStatus appendDocumentNode(const node_id parentID,
                             const DocumentNode *docNode, Dom *dom,
                             DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseDocumentElement(docNode, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse document element!\n");
        return domStatus;
    }
    return updateReferences(parentID, newNodeID, dom);
}

DomStatus appendTextNode(const node_id parentID, const char *text, Dom *dom,
                         DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseTextElement(text, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse text element!\n");
        return domStatus;
    }

    node_id child = getFirstChild(parentID, dom);
    if (child > 0) {
        child = getLastNext(child, dom);

        MergeResult mergeTry =
            tryMerge(&dom->nodes[child], &dom->nodes[newNodeID], dom,
                     dataContainer, true);

        if (mergeTry == COMPLETED_MERGE) {
            removeNode(newNodeID, dom);
            return domStatus;
        }

        if (mergeTry == FAILED_MERGE) {
            return DOM_NO_ADD;
        }
    }

    return updateReferences(parentID, newNodeID, dom);
}

DomStatus appendNodesFromString(const node_id parentID, const char *htmlString,
                                Dom *dom, DataContainer *dataContainer) {
    node_id firstNewAddedNode = dom->nodeLen;
    DomStatus domStatus = parse(htmlString, dom, dataContainer);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse string!\n");
        return domStatus;
    }

    node_id firstChild = getFirstChild(parentID, dom);
    if (firstChild > 0) {
        node_id lastNextNode = getLastNext(firstNewAddedNode, dom);
        if (lastNextNode > firstNewAddedNode) {
            Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
            if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
                node_id lastNext = getLastNext(firstChild, dom);
                MergeResult mergeResult =
                    tryMerge(&dom->nodes[lastNext], firstAddedNode, dom,
                             dataContainer, true);
                if (mergeResult == COMPLETED_MERGE) {
                    size_t secondNewAddedNode = getNext(firstNewAddedNode, dom);
                    removeNode(firstNewAddedNode, dom);
                    firstNewAddedNode = secondNewAddedNode;
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        } else {
            Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
            if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
                node_id lastNext = getLastNext(firstChild, dom);
                MergeResult mergeResult =
                    tryMerge(&dom->nodes[lastNext], firstAddedNode, dom,
                             dataContainer, true);
                if (mergeResult == COMPLETED_MERGE) {
                    removeNode(firstNewAddedNode, dom);
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
