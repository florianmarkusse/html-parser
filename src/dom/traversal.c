
#include "flo/html-parser/dom/registry.h"

// TODO(florian): make faster.
ParentChild *getFirstChildNode(const node_id currentNodeID, const Dom *dom) {
    for (node_id i = 0; i < dom->parentFirstChildLen; i++) {
        if (dom->parentFirstChilds[i].parentID == currentNodeID) {
            return &dom->parentFirstChilds[i];
        }
    }
    return NULL;
}

node_id getFirstChild(const node_id currentNodeID, const Dom *dom) {
    ParentChild *firstChild = getFirstChildNode(currentNodeID, dom);
    if (firstChild == NULL) {
        return 0;
    }

    return firstChild->childID;
}

// TODO(florian): make faster.
NextNode *getNextNode(const node_id currentNodeID, const Dom *dom) {
    for (node_id i = 0; i < dom->nextNodeLen; i++) {
        if (dom->nextNodes[i].currentNodeID == currentNodeID) {
            return &dom->nextNodes[i];
        }
    }
    return NULL;
}

node_id getNext(const node_id currentNodeID, const Dom *dom) {
    NextNode *nextNode = getNextNode(currentNodeID, dom);
    if (nextNode == NULL) {
        return 0;
    }

    return nextNode->nextNodeID;
}

// TODO(florian): make faster.
ParentChild *getParentNode(const node_id currentNodeID, const Dom *dom) {
    for (size_t i = 0; i < dom->parentChildLen; i++) {
        ParentChild *node = &dom->parentChilds[i];
        if (node->childID == currentNodeID) {
            return node;
        }
    }
    return NULL;
}

node_id getParent(const node_id currentNodeID, const Dom *dom) {
    ParentChild *parentChildNode = getParentNode(currentNodeID, dom);
    if (parentChildNode == NULL) {
        return 0;
    }
    return parentChildNode->parentID;
}

NextNode *getPreviousNode(const node_id currentNodeID, const Dom *dom) {
    const node_id parentNodeID = getParent(currentNodeID, dom);
    if (parentNodeID > 0) {
        ParentChild *parentFirstChildNode =
            getFirstChildNode(parentNodeID, dom);
        if (parentFirstChildNode->childID == currentNodeID) {
            return NULL;
        }

        NextNode *nextNode = getNextNode(parentFirstChildNode->childID, dom);
        while (nextNode->nextNodeID != currentNodeID) {
            nextNode = getNextNode(nextNode->nextNodeID, dom);
        }

        return nextNode;
    }

    if (dom->firstNodeID == currentNodeID) {
        return NULL;
    }

    NextNode *nextNode = getNextNode(dom->firstNodeID, dom);
    while (nextNode->nextNodeID != currentNodeID) {
        nextNode = getNextNode(nextNode->nextNodeID, dom);
    }

    return nextNode;
}

node_id getPrevious(const node_id currentNodeID, const Dom *dom) {
    NextNode *previousNode = getPreviousNode(currentNodeID, dom);
    if (previousNode == NULL) {
        return 0;
    }
    return previousNode->currentNodeID;
}

node_id traverseNode(const node_id currentNodeID,
                     const node_id toTraverseNodeID, const Dom *dom) {
    node_id firstChild = getFirstChild(currentNodeID, dom);
    if (firstChild) {
        return firstChild;
    }

    node_id nextNode = getNext(currentNodeID, dom);
    if (nextNode > 0 && nextNode != toTraverseNodeID) {
        return nextNode;
    }

    if (currentNodeID != toTraverseNodeID) {
        node_id parentNodeID = getParent(currentNodeID, dom);
        while (parentNodeID > 0 && parentNodeID != toTraverseNodeID) {
            node_id parentsNextNode = getNext(parentNodeID, dom);
            if (parentsNextNode) {
                return parentsNextNode;
            }
            parentNodeID = getParent(parentNodeID, dom);
        }
    }

    return 0;
}

node_id traverseDom(const node_id currentNodeID, const Dom *dom) {
    node_id firstChild = getFirstChild(currentNodeID, dom);
    if (firstChild) {
        return firstChild;
    }

    node_id nextNode = getNext(currentNodeID, dom);
    if (nextNode) {
        return nextNode;
    }

    node_id parentNodeID = getParent(currentNodeID, dom);
    while (parentNodeID) {
        node_id parentsNextNode = getNext(parentNodeID, dom);
        if (parentsNextNode) {
            return parentsNextNode;
        }
        parentNodeID = getParent(parentNodeID, dom);
    }

    return 0;
}