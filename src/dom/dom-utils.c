
#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/type/node/tag-registration.h"

const char *getTag(const indexID tagID, const Dom *dom,
                   const DataContainer *dataContainer) {
    TagRegistration *tagRegistration = &dom->tagRegistry[tagID];
    return getStringFromHashSet(&dataContainer->tags.set,
                                &tagRegistration->hashElement);
}

void getTagRegistration(indexID tagID, const Dom *dom,
                        TagRegistration **tagRegistration) {
    *tagRegistration = &dom->tagRegistry[tagID];
}

const char *getBoolProp(const indexID boolPropID, const Dom *dom,
                        const DataContainer *dataContainer) {
    Registration registration = dom->boolPropRegistry.registry[boolPropID];
    return getStringFromHashSet(&dataContainer->boolProps.set,
                                &registration.hashElement);
}

const char *getPropKey(const indexID propKeyID, const Dom *dom,
                       const DataContainer *dataContainer) {
    Registration registration = dom->propKeyRegistry.registry[propKeyID];
    return getStringFromHashSet(&dataContainer->propKeys.set,
                                &registration.hashElement);
}

const char *getPropValue(const indexID propValueID, const Dom *dom,
                         const DataContainer *dataContainer) {
    Registration registration = dom->propValueRegistry.registry[propValueID];
    return getStringFromHashSet(&dataContainer->propValues.set,
                                &registration.hashElement);
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

node_id traverseNode(node_id currentNodeID, const node_id toTraverseNodeID,
                     const Dom *dom) {
    node_id firstChild = getFirstChild(currentNodeID, dom);
    if (firstChild) {
        return firstChild;
    }

    node_id nextNode = getNextNode(currentNodeID, dom);
    if (nextNode > 0 && nextNode != toTraverseNodeID) {
        return nextNode;
    }

    if (currentNodeID != toTraverseNodeID) {
        node_id parentNodeID = getParentNode(currentNodeID, dom);
        while (parentNodeID > 0 && parentNodeID != toTraverseNodeID) {
            node_id parentsNextNode = getNextNode(parentNodeID, dom);
            if (parentsNextNode) {
                return parentsNextNode;
            }
            parentNodeID = getParentNode(parentNodeID, dom);
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
