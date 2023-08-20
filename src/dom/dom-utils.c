
#include "flo/html-parser/dom/dom-utils.h"

const char *getText(const node_id nodeID, const Dom *dom,
                    const DataContainer *dataContainer) {
    for (size_t i = 0; i < dom->textLen; i++) {
        TextNode textNode = dom->text[i];

        if (textNode.nodeID == nodeID) {
            char *text =
                dataContainer->text.container.elements[textNode.textID];
            return text;
        }
    }

    return NULL;
}

// TODO(florian): make faster.
node_id getFirstChild(const node_id parentID, const Dom *dom) {
    for (node_id i = 0; i < dom->parentFirstChildLen; i++) {
        if (dom->parentFirstChilds[i].parentID == parentID) {
            return dom->parentFirstChilds[i].childID;
        }
    }
    return 0;
}

// TODO(florian): make faster.
node_id getNextNode(const node_id currentNodeID, const Dom *dom) {
    for (node_id i = 0; i < dom->nextNodeLen; i++) {
        if (dom->nextNodes[i].currentNodeID == currentNodeID) {
            return dom->nextNodes[i].nextNodeID;
        }
    }
    return 0;
}

// TODO(florian): make faster.
node_id getParentNode(node_id currentNodeID, const Dom *dom) {
    for (size_t i = 0; i < dom->parentChildLen; i++) {
        ParentChild node = dom->parentChilds[i];
        if (node.childID == currentNodeID) {
            return node.parentID;
        }
    }

    return 0;
}

node_id traverseDom(node_id currentNodeID, const Dom *dom) {
    node_id firstChild = getFirstChild(currentNodeID, dom);
    if (firstChild) {
        return firstChild;
    }

    node_id nextNode = getNextNode(currentNodeID, dom);
    if (nextNode) {
        return nextNode;
    }

    node_id parentNodeID = getParentNode(currentNodeID, dom);
    while (parentNodeID) {
        node_id parentsNextNode = getNextNode(parentNodeID, dom);
        if (parentsNextNode) {
            return parentsNextNode;
        }
        parentNodeID = getParentNode(parentNodeID, dom);
    }

    return 0;
}
