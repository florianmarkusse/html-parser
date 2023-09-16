
#include "flo/html-parser/dom/registry.h"

// TODO(florian): make faster.
flo_html_ParentChild *
flo_html_getFirstChildNode(const flo_html_node_id currentNodeID,
                           const flo_html_Dom *dom) {
    for (flo_html_node_id i = 0; i < dom->parentFirstChildLen; i++) {
        if (dom->parentFirstChilds[i].parentID == currentNodeID) {
            return &dom->parentFirstChilds[i];
        }
    }
    return NULL;
}

flo_html_node_id flo_html_getFirstChild(const flo_html_node_id currentNodeID,
                                        const flo_html_Dom *dom) {
    if (currentNodeID == 0) {
        return dom->firstNodeID;
    }
    flo_html_ParentChild *firstChild =
        flo_html_getFirstChildNode(currentNodeID, dom);
    if (firstChild == NULL) {
        return 0;
    }

    return firstChild->childID;
}

// TODO(florian): make faster.
flo_html_NextNode *flo_html_getNextNode(const flo_html_node_id currentNodeID,
                                        const flo_html_Dom *dom) {
    for (flo_html_node_id i = 0; i < dom->nextNodeLen; i++) {
        if (dom->nextNodes[i].currentNodeID == currentNodeID) {
            return &dom->nextNodes[i];
        }
    }
    return NULL;
}

flo_html_node_id flo_html_getNext(const flo_html_node_id currentNodeID,
                                  const flo_html_Dom *dom) {
    flo_html_NextNode *nextNode = flo_html_getNextNode(currentNodeID, dom);
    if (nextNode == NULL) {
        return 0;
    }

    return nextNode->nextNodeID;
}

// TODO(florian): make faster.
flo_html_ParentChild *
flo_html_getParentNode(const flo_html_node_id currentNodeID,
                       const flo_html_Dom *dom) {
    for (size_t i = 0; i < dom->parentChildLen; i++) {
        flo_html_ParentChild *node = &dom->parentChilds[i];
        if (node->childID == currentNodeID) {
            return node;
        }
    }
    return NULL;
}

flo_html_node_id flo_html_getParent(const flo_html_node_id currentNodeID,
                                    const flo_html_Dom *dom) {
    flo_html_ParentChild *parentChildNode =
        flo_html_getParentNode(currentNodeID, dom);
    if (parentChildNode == NULL) {
        return 0;
    }
    return parentChildNode->parentID;
}

flo_html_NextNode *
flo_html_getPreviousNode(const flo_html_node_id currentNodeID,
                         const flo_html_Dom *dom) {
    for (flo_html_node_id i = 0; i < dom->nextNodeLen; i++) {
        if (dom->nextNodes[i].nextNodeID == currentNodeID) {
            return &dom->nextNodes[i];
        }
    }
    return NULL;
}

flo_html_node_id flo_html_getPrevious(const flo_html_node_id currentNodeID,
                                      const flo_html_Dom *dom) {
    flo_html_NextNode *previousNode =
        flo_html_getPreviousNode(currentNodeID, dom);
    if (previousNode == NULL) {
        return 0;
    }
    return previousNode->currentNodeID;
}

flo_html_node_id flo_html_traverseNode(const flo_html_node_id currentNodeID,
                                       const flo_html_node_id toTraverseNodeID,
                                       const flo_html_Dom *dom) {
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

flo_html_node_id flo_html_traverseDom(const flo_html_node_id currentNodeID,
                                      const flo_html_Dom *dom) {
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

flo_html_node_id flo_html_getLastNext(const flo_html_node_id startNodeID,
                                      const flo_html_Dom *dom) {
    flo_html_node_id lastNext = startNodeID;
    flo_html_NextNode *nextNode = flo_html_getNextNode(lastNext, dom);
    while (nextNode != NULL) {
        lastNext = nextNode->nextNodeID;
        nextNode = flo_html_getNextNode(lastNext, dom);
    }

    return lastNext;
}

flo_html_NextNode *flo_html_getLastNextNode(const flo_html_node_id startNodeID,
                                            const flo_html_Dom *dom) {
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
