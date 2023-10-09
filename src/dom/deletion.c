#include <stdio.h>

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/reading/reading-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/node/boolean-property.h"

void flo_html_removeNode(const flo_html_node_id nodeID, flo_html_Dom *dom) {
    flo_html_removeChildren(nodeID, dom);

    flo_html_Node *node = &dom->nodes[nodeID];
    flo_html_NextNode *nextNode = flo_html_getNextNode(node->nodeID, dom);

    if (dom->firstNodeID == nodeID) {
        dom->firstNodeID = flo_html_getNext(dom->firstNodeID, dom);
        if (nextNode != NULL) {
            nextNode->nextNodeID = 0;
            nextNode->currentNodeID = 0;
        }
        node->nodeID = 0;
        node->nodeType = NODE_TYPE_REMOVED;
        return;
    }

    flo_html_ParentChild *parentChildNode =
        flo_html_getParentNode(node->nodeID, dom);
    // Order of modifications is important here.
    // getPreviousNode makes use of the parent node to get the previous node.
    flo_html_NextNode *previousNode =
        flo_html_getPreviousNode(node->nodeID, dom);
    if (previousNode != NULL) {
        if (nextNode == NULL) {
            previousNode->currentNodeID = 0;
            previousNode->nextNodeID = 0;
        } else {
            previousNode->nextNodeID = nextNode->nextNodeID;
        }
    }

    if (parentChildNode != NULL) {
        flo_html_node_id parentID = parentChildNode->parentID;

        parentChildNode->parentID = 0;
        parentChildNode->childID = 0;

        parentChildNode = flo_html_getFirstChildNode(parentID, dom);

        if (parentChildNode->childID == nodeID) {
            if (nextNode == NULL) {
                parentChildNode->parentID = 0;
                parentChildNode->childID = 0;
            } else {
                parentChildNode->childID = nextNode->nextNodeID;
            }
        }
    }

    if (nextNode != NULL) {
        nextNode->nextNodeID = 0;
        nextNode->currentNodeID = 0;
    }
    node->tagID = 0;
    node->nodeType = NODE_TYPE_REMOVED;
}

// TODO(florian): improve the speed
// Either make it so we can just mark nodes as DELETED and then they wont show
// up in query resuls or printing.
void flo_html_removeChildren(const flo_html_node_id nodeID, flo_html_Dom *dom) {
    flo_html_node_id childID = flo_html_getFirstChild(nodeID, dom);
    while (childID > 0) {
        flo_html_removeNode(childID, dom);
        childID = flo_html_getFirstChild(nodeID, dom);
    }
}

void flo_html_removeBooleanProperty(const flo_html_node_id nodeID,
                                    const flo_html_String boolProp,
                                    flo_html_Dom *dom,
                                    const flo_html_TextStore *textStore) {
    flo_html_index_id boolPropID = flo_html_getBoolPropID(boolProp, textStore);
    if (boolPropID > 0) {
        flo_html_BooleanProperty *prop =
            flo_html_getBooleanProperty(nodeID, boolPropID, dom);
        if (prop != NULL) {
            prop->nodeID = 0;
            prop->propID = 0;
        }
    }
}

void flo_html_removeProperty(const flo_html_node_id nodeID,
                             const flo_html_String keyProp, flo_html_Dom *dom,
                             const flo_html_TextStore *textStore) {
    flo_html_index_id keyPropID = flo_html_getPropKeyID(keyProp, textStore);
    if (keyPropID > 0) {
        flo_html_Property *prop = flo_html_getProperty(nodeID, keyPropID, dom);
        if (prop != NULL) {
            prop->nodeID = 0;
            prop->keyID = 0;
            prop->valueID = 0;
        }
    }
}
