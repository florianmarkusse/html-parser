
#include "flo/html-parser/dom/traversal.h"

// TODO(florian): make faster.
flo_html_ParentChild *flo_html_getFirstChildNode(flo_html_node_id currentNodeID,
                                                 flo_html_Dom *dom) {
    for (flo_html_node_id i = 0; i < dom->parentFirstChilds.len; i++) {
        if (dom->parentFirstChilds.buf[i].parentID == currentNodeID) {
            return &dom->parentFirstChilds.buf[i];
        }
    }
    return NULL;
}

flo_html_node_id flo_html_getFirstChild(flo_html_node_id currentNodeID,
                                        flo_html_Dom *dom) {
    flo_html_ParentChild *firstChild =
        flo_html_getFirstChildNode(currentNodeID, dom);
    if (firstChild == NULL) {
        return 0;
    }

    return firstChild->childID;
}

// TODO(florian): make faster.
flo_html_NextNode *flo_html_getNextNode(flo_html_node_id currentNodeID,
                                        flo_html_Dom *dom) {
    for (flo_html_node_id i = 0; i < dom->nextNodes.len; i++) {
        if (dom->nextNodes.buf[i].currentNodeID == currentNodeID) {
            return &dom->nextNodes.buf[i];
        }
    }
    return NULL;
}

flo_html_node_id flo_html_getNext(flo_html_node_id currentNodeID,
                                  flo_html_Dom *dom) {
    flo_html_NextNode *nextNode = flo_html_getNextNode(currentNodeID, dom);
    if (nextNode == NULL) {
        return 0;
    }

    return nextNode->nextNodeID;
}

// TODO(florian): make faster.
flo_html_ParentChild *flo_html_getParentNode(flo_html_node_id currentNodeID,
                                             flo_html_Dom *dom) {
    for (ptrdiff_t i = 0; i < dom->parentChilds.len; i++) {
        flo_html_ParentChild *node = &dom->parentChilds.buf[i];
        if (node->childID == currentNodeID) {
            return node;
        }
    }
    return NULL;
}

flo_html_node_id flo_html_getParent(flo_html_node_id currentNodeID,
                                    flo_html_Dom *dom) {
    flo_html_ParentChild *parentChildNode =
        flo_html_getParentNode(currentNodeID, dom);
    if (parentChildNode == NULL) {
        return 0;
    }
    return parentChildNode->parentID;
}

flo_html_NextNode *flo_html_getPreviousNode(flo_html_node_id currentNodeID,
                                            flo_html_Dom *dom) {
    for (flo_html_node_id i = 0; i < dom->nextNodes.len; i++) {
        if (dom->nextNodes.buf[i].nextNodeID == currentNodeID) {
            return &dom->nextNodes.buf[i];
        }
    }
    return NULL;
}

flo_html_node_id flo_html_getPrevious(flo_html_node_id currentNodeID,
                                      flo_html_Dom *dom) {
    flo_html_NextNode *previousNode =
        flo_html_getPreviousNode(currentNodeID, dom);
    if (previousNode == NULL) {
        return 0;
    }
    return previousNode->currentNodeID;
}

flo_html_node_id flo_html_traverseNode(flo_html_node_id currentNodeID,
                                       flo_html_node_id toTraverseNodeID,
                                       flo_html_Dom *dom) {
    flo_html_node_id firstChild = flo_html_getFirstChild(currentNodeID, dom);
    if (firstChild) {
        return firstChild;
    }

    flo_html_node_id nextNode = flo_html_getNext(currentNodeID, dom);
    if (nextNode > 0 && nextNode != toTraverseNodeID) {
        return nextNode;
    }

    if (currentNodeID != toTraverseNodeID) {
        flo_html_node_id parentNodeID = flo_html_getParent(currentNodeID, dom);
        while (parentNodeID > 0 && parentNodeID != toTraverseNodeID) {
            flo_html_node_id parentsNextNode =
                flo_html_getNext(parentNodeID, dom);
            if (parentsNextNode) {
                return parentsNextNode;
            }
            parentNodeID = flo_html_getParent(parentNodeID, dom);
        }
    }

    return 0;
}

flo_html_node_id flo_html_traverseDom(flo_html_node_id currentNodeID,
                                      flo_html_Dom *dom) {
    flo_html_node_id firstChild = flo_html_getFirstChild(currentNodeID, dom);
    if (firstChild) {
        return firstChild;
    }

    flo_html_node_id nextNode = flo_html_getNext(currentNodeID, dom);
    if (nextNode) {
        return nextNode;
    }

    flo_html_node_id parentNodeID = flo_html_getParent(currentNodeID, dom);
    while (parentNodeID) {
        flo_html_node_id parentsNextNode = flo_html_getNext(parentNodeID, dom);
        if (parentsNextNode) {
            return parentsNextNode;
        }
        parentNodeID = flo_html_getParent(parentNodeID, dom);
    }

    return 0;
}

flo_html_node_id flo_html_getLastNext(flo_html_node_id startNodeID,
                                      flo_html_Dom *dom) {
    flo_html_node_id lastNext = startNodeID;
    flo_html_NextNode *nextNode = flo_html_getNextNode(lastNext, dom);
    while (nextNode != NULL) {
        lastNext = nextNode->nextNodeID;
        nextNode = flo_html_getNextNode(lastNext, dom);
    }

    return lastNext;
}

flo_html_NextNode *flo_html_getLastNextNode(flo_html_node_id startNodeID,
                                            flo_html_Dom *dom) {
    flo_html_NextNode *nextNode = flo_html_getNextNode(startNodeID, dom);
    if (nextNode == NULL) {
        return NULL;
    }

    ;
    flo_html_NextNode *pastNextNode =
        flo_html_getNextNode(nextNode->nextNodeID, dom);
    while (pastNextNode != NULL) {
        nextNode = pastNextNode;
        pastNextNode = flo_html_getNextNode(pastNextNode->nextNodeID, dom);
    }

    return nextNode;
}
