
#include <string.h>

#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/replacement/replacement.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/utils.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/print/error.h"

static void updateReferences(const node_id toReplaceNodeID,
                             const node_id newNodeID, Dom *dom) {
    Node *nodeToReplace = &dom->nodes[toReplaceNodeID];
    const node_id lastNextOfNew = getLastNext(newNodeID, dom);

    if (dom->firstNodeID == toReplaceNodeID) {
        NextNode *nextNode = getNextNode(toReplaceNodeID, dom);
        if (nextNode != NULL) {
            nextNode->currentNodeID = lastNextOfNew;
        }

        dom->firstNodeID = newNodeID;
        nodeToReplace->nodeType = NODE_TYPE_REMOVED;
        return;
    }

    ParentChild *parentChildNode = getParentNode(toReplaceNodeID, dom);
    ParentChild *parentFirstChildNode = NULL;
    if (parentChildNode != NULL) {
        parentFirstChildNode =
            getFirstChildNode(parentChildNode->parentID, dom);
    }

    NextNode *previousNext = getPreviousNode(toReplaceNodeID, dom);
    NextNode *nextNode = getNextNode(toReplaceNodeID, dom);

    // If there is no previous next node, it must mean we either are the first
    // child or are a root element
    if (previousNext != NULL) {
        previousNext->nextNodeID = newNodeID;
    } else {
        if (parentFirstChildNode != NULL) {
            parentFirstChildNode->childID = newNodeID;
        }
    }

    if (parentChildNode != NULL) {
        parentChildNode->childID = newNodeID;
        node_id otherNewNodeID = getNext(newNodeID, dom);
        while (otherNewNodeID > 0) {
            addParentChild(parentFirstChildNode->parentID, otherNewNodeID, dom);
            otherNewNodeID = getNext(otherNewNodeID, dom);
        }
    }

    if (nextNode != NULL) {
        nextNode->currentNodeID = lastNextOfNew;
    }

    nodeToReplace->nodeType = NODE_TYPE_REMOVED;
}

DomStatus replaceWithDocumentNode(node_id toReplaceNodeID,
                                  const DocumentNode *docNode, Dom *dom,
                                  DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseDocumentElement(docNode, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse document element!\n");
        return domStatus;
    }
    updateReferences(toReplaceNodeID, newNodeID, dom);
    return domStatus;
}

MergeResult tryMergeBothSides(const node_id toReplaceNodeID,
                              const node_id replacingNodeID, Dom *dom,
                              DataContainer *dataContainer) {
    Node *replacingNode = &dom->nodes[replacingNodeID];
    if (replacingNode->nodeType == NODE_TYPE_TEXT) {
        NextNode *previousNode = getPreviousNode(toReplaceNodeID, dom);
        if (previousNode != NULL) {
            MergeResult mergeTry =
                tryMerge(&dom->nodes[previousNode->currentNodeID],
                         replacingNode, dom, dataContainer);
            if (mergeTry == FAILED_MERGE || mergeTry == COMPLETED_MERGE) {
                return mergeTry;
            }
        }

        NextNode *nextNode = getNextNode(toReplaceNodeID, dom);
        if (nextNode != NULL) {
            MergeResult mergeTry = tryMerge(&dom->nodes[nextNode->nextNodeID],
                                            replacingNode, dom, dataContainer);
            return mergeTry;
        }
    }

    return NO_MERGE;
}

DomStatus replaceWithTextNode(node_id toReplaceNodeID, const char *text,
                              Dom *dom, DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus =
        parseTextElement(text, dom, dataContainer, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse text element!\n");
        return domStatus;
    }

    MergeResult mergeResult =
        tryMergeBothSides(toReplaceNodeID, newNodeID, dom, dataContainer);
    if (mergeResult == COMPLETED_MERGE) {
        removeNode(newNodeID, dom);
        removeNode(toReplaceNodeID, dom);
        return domStatus;
    }

    if (mergeResult == FAILED_MERGE) {
        return DOM_NO_ADD;
    }

    updateReferences(toReplaceNodeID, newNodeID, dom);
    return domStatus;
}

DomStatus replaceWithNodesFromString(node_id toReplaceNodeID,
                                     const char *htmlString, Dom *dom,
                                     DataContainer *dataContainer) {
    node_id firstNewAddedNode = dom->nodeLen;
    DomStatus domStatus = parse(htmlString, dom, dataContainer);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse string!\n");
        return domStatus;
    }

    node_id lastNextNode = getLastNext(firstNewAddedNode, dom);
    if (lastNextNode > firstNewAddedNode) {
        Node *firstAddedNode = &dom->nodes[firstNewAddedNode];
        if (firstAddedNode->nodeType == NODE_TYPE_TEXT) {
            NextNode *previousNode = getPreviousNode(toReplaceNodeID, dom);
            if (previousNode != NULL) {
                MergeResult mergeResult =
                    tryMerge(&dom->nodes[previousNode->currentNodeID],
                             firstAddedNode, dom, dataContainer);
                if (mergeResult == COMPLETED_MERGE) {
                    size_t secondNewAddedNode = getNext(firstNewAddedNode, dom);
                    removeNode(firstNewAddedNode, dom);
                    firstNewAddedNode = secondNewAddedNode;
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        }

        Node *lastAddedNode = &dom->nodes[lastNextNode];
        if (lastAddedNode->nodeType == NODE_TYPE_TEXT) {
            NextNode *nextNode = getNextNode(toReplaceNodeID, dom);
            if (nextNode != NULL) {
                MergeResult mergeResult =
                    tryMerge(&dom->nodes[nextNode->nextNodeID],
                             &dom->nodes[lastNextNode], dom, dataContainer);

                if (mergeResult == COMPLETED_MERGE) {
                    removeNode(lastNextNode, dom);
                }

                if (mergeResult == FAILED_MERGE) {
                    return DOM_NO_ADD;
                }
            }
        }
    } else {
        MergeResult mergeResult = tryMergeBothSides(
            toReplaceNodeID, firstNewAddedNode, dom, dataContainer);
        if (mergeResult == COMPLETED_MERGE) {
            removeNode(firstNewAddedNode, dom);
            removeNode(toReplaceNodeID, dom);
            return domStatus;
        }

        if (mergeResult == FAILED_MERGE) {
            return DOM_NO_ADD;
        }
    }

    updateReferences(toReplaceNodeID, firstNewAddedNode, dom);
    return domStatus;
}
