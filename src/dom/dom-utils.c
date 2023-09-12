#include <stdbool.h>
#include <string.h>

#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/tag-registration.h"
#include "flo/html-parser/utils/memory/memory.h"
#include "flo/html-parser/utils/print/error.h"

DomStatus createNode(node_id *nodeID, const NodeType nodeType, Dom *dom) {
    if ((dom->nodes = resizeArray(dom->nodes, dom->nodeLen, &dom->nodeCap,
                                  sizeof(Node), NODES_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    Node *newNode = &(dom->nodes[dom->nodeLen]);
    newNode->nodeType = nodeType;
    newNode->nodeID = dom->nodeLen;
    dom->nodeLen++;

    if (dom->firstNodeID == 0) {
        dom->firstNodeID = newNode->nodeID;
    }

    *nodeID = newNode->nodeID;
    return DOM_SUCCESS;
}

void setNodeTagID(const node_id nodeID, const indexID tagID, Dom *dom) {
    Node *createdNode = &(dom->nodes[nodeID]);
    createdNode->tagID = tagID;
}

void setNodeText(const node_id nodeID, const char *text, Dom *dom) {
    Node *createdNode = &(dom->nodes[nodeID]);
    createdNode->text = text;
}

DomStatus addParentFirstChild(const node_id parentID, const node_id childID,
                              Dom *dom) {
    if ((dom->parentFirstChilds =
             resizeArray(dom->parentFirstChilds, dom->parentFirstChildLen,
                         &dom->parentFirstChildCap, sizeof(ParentChild),
                         PARENT_FIRST_CHILDS_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    ParentChild *newParentFirstChild =
        &(dom->parentFirstChilds[dom->parentFirstChildLen]);
    newParentFirstChild->parentID = parentID;
    newParentFirstChild->childID = childID;
    dom->parentFirstChildLen++;
    return DOM_SUCCESS;
}

DomStatus addParentChild(const node_id parentID, const node_id childID,
                         Dom *dom) {
    if ((dom->parentChilds = resizeArray(
             dom->parentChilds, dom->parentChildLen, &dom->parentChildCap,
             sizeof(ParentChild), PARENT_CHILDS_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    ParentChild *newParentChild = &(dom->parentChilds[dom->parentChildLen]);
    newParentChild->parentID = parentID;
    newParentChild->childID = childID;
    dom->parentChildLen++;
    return DOM_SUCCESS;
}

DomStatus addNextNode(const node_id currentNodeID, const node_id nextNodeID,
                      Dom *dom) {
    if ((dom->nextNodes =
             resizeArray(dom->nextNodes, dom->nextNodeLen, &dom->nextNodeCap,
                         sizeof(NextNode), NEXT_NODES_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    NextNode *newNextNode = &(dom->nextNodes[dom->nextNodeLen]);
    newNextNode->currentNodeID = currentNodeID;
    newNextNode->nextNodeID = nextNodeID;
    dom->nextNodeLen++;
    return DOM_SUCCESS;
}

DomStatus addBooleanProperty(const node_id nodeID, const element_id propID,
                             Dom *dom) {
    if ((dom->boolProps = resizeArray(
             dom->boolProps, dom->boolPropsLen, &dom->boolPropsCap,
             sizeof(BooleanProperty), BOOLEAN_PROPERTIES_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    BooleanProperty *newBooleanProperty = &(dom->boolProps[dom->boolPropsLen]);
    newBooleanProperty->nodeID = nodeID;
    newBooleanProperty->propID = propID;
    dom->boolPropsLen++;
    return DOM_SUCCESS;
}

DomStatus addProperty(const node_id nodeID, const element_id keyID,
                      const element_id valueID, Dom *dom) {
    if ((dom->props = resizeArray(dom->props, dom->propsLen, &dom->propsCap,
                                  sizeof(Property), PROPERTIES_PER_PAGE)) ==
        NULL) {
        return DOM_ERROR_MEMORY;
    }

    Property *newProperty = &(dom->props[dom->propsLen]);
    newProperty->nodeID = nodeID;
    newProperty->keyID = keyID;
    newProperty->valueID = valueID;
    dom->propsLen++;
    return DOM_SUCCESS;
}

const char *getTag(const indexID tagID, const Dom *dom,
                   const TextStore *textStore) {
    TagRegistration *tagRegistration = &dom->tagRegistry[tagID];
    return getStringFromHashSet(&textStore->tags.set,
                                &tagRegistration->hashElement);
}

void getTagRegistration(indexID tagID, const Dom *dom,
                        TagRegistration **tagRegistration) {
    *tagRegistration = &dom->tagRegistry[tagID];
}

const char *getBoolProp(const indexID boolPropID, const Dom *dom,
                        const TextStore *textStore) {
    Registration registration = dom->boolPropRegistry.registry[boolPropID];
    return getStringFromHashSet(&textStore->boolProps.set,
                                &registration.hashElement);
}

const char *getPropKey(const indexID propKeyID, const Dom *dom,
                       const TextStore *textStore) {
    Registration registration = dom->propKeyRegistry.registry[propKeyID];
    return getStringFromHashSet(&textStore->propKeys.set,
                                &registration.hashElement);
}

const char *getPropValue(const indexID propValueID, const Dom *dom,
                         const TextStore *textStore) {
    Registration registration = dom->propValueRegistry.registry[propValueID];
    return getStringFromHashSet(&textStore->propValues.set,
                                &registration.hashElement);
}

MergeResult tryMerge(Node *possibleMergeNode, Node *replacingNode, Dom *dom,
                     TextStore *textStore, bool isAppend) {
    if (possibleMergeNode->nodeType == NODE_TYPE_TEXT) {
        ElementStatus elementStatus = addTextToTextNode(
            possibleMergeNode, replacingNode->text, strlen(replacingNode->text),
            dom, textStore, isAppend);
        if (elementStatus != ELEMENT_CREATED) {
            PRINT_ERROR("Failed to merge new text node with up node!\n");
            return FAILED_MERGE;
        }
        return COMPLETED_MERGE;
    }
    return NO_MERGE;
}

DomStatus connectOtherNodesToParent(const node_id parentID,
                                    const node_id lastAddedChild, Dom *dom) {
    node_id otherNewNodeID = getNext(lastAddedChild, dom);
    while (otherNewNodeID > 0) {
        DomStatus domStatus = addParentChild(parentID, otherNewNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID as child!\n");
            return domStatus;
        }
        otherNewNodeID = getNext(otherNewNodeID, dom);
    }

    return DOM_SUCCESS;
}
