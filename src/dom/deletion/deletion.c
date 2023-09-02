#include <stdio.h>

#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/traversal.h"

void removeNode(const node_id nodeID, Dom *dom) {
    Node *node = &dom->nodes[nodeID];
    NextNode *nextNode = getNextNode(node->nodeID, dom);

    if (dom->firstNodeID == nodeID) {
        dom->firstNodeID = getNext(dom->firstNodeID, dom);
        if (nextNode != NULL) {
            nextNode->nextNodeID = 0;
            nextNode->currentNodeID = 0;
        }
        node->nodeType = NODE_TYPE_REMOVED;
        return;
    }

    node_id parentID = getParent(node->nodeID, dom);

    // Order of modifications is important here.
    // getPreviousNode makes use of the parent node to get the previous node.
    NextNode *previousNode = getPreviousNode(node->nodeID, dom);
    if (previousNode != NULL) {
        if (nextNode == NULL) {
            previousNode->currentNodeID = 0;
            previousNode->nextNodeID = 0;
        } else {
            previousNode->nextNodeID = nextNode->nextNodeID;
        }
    }

    if (parentID > 0) {
        ParentChild *parentChildNode = getFirstChildNode(parentID, dom);
        if (parentChildNode->childID == nodeID) {
            if (nextNode == NULL) {
                parentChildNode->parentID = 0;
                parentChildNode->childID = 0;
            } else {
                parentChildNode->childID = nextNode->nextNodeID;
            }
        }

        parentChildNode = getParentNode(node->nodeID, dom);
        parentChildNode->parentID = 0;
        parentChildNode->childID = 0;
    }

    if (nextNode != NULL) {
        nextNode->nextNodeID = 0;
        nextNode->currentNodeID = 0;
    }
    node->nodeType = NODE_TYPE_REMOVED;
}
