#include <stdbool.h>
#include <string.h>

#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/node/tag-registration.h"
#include "flo/html-parser/utils/memory/memory.h"
#include "flo/html-parser/utils/print/error.h"

flo_html_DomStatus flo_html_createNode(flo_html_node_id *nodeID,
                                       const flo_html_NodeType nodeType,
                                       flo_html_Dom *dom) {
    if ((dom->nodes = flo_html_resizeArray(dom->nodes, dom->nodeLen, &dom->nodeCap,
                                  sizeof(flo_html_Node),
                                  FLO_HTML_NODES_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    flo_html_Node *newNode = &(dom->nodes[dom->nodeLen]);
    newNode->nodeType = nodeType;
    newNode->nodeID = dom->nodeLen;
    dom->nodeLen++;

    if (dom->firstNodeID == 0) {
        dom->firstNodeID = newNode->nodeID;
    }

    *nodeID = newNode->nodeID;
    return DOM_SUCCESS;
}

void flo_html_setNodeTagID(const flo_html_node_id nodeID,
                           const flo_html_indexID tagID, flo_html_Dom *dom) {
    flo_html_Node *createdNode = &(dom->nodes[nodeID]);
    createdNode->tagID = tagID;
}

void flo_html_setNodeText(const flo_html_node_id nodeID, const char *text,
                          flo_html_Dom *dom) {
    flo_html_Node *createdNode = &(dom->nodes[nodeID]);
    createdNode->text = text;
}

flo_html_DomStatus flo_html_addParentFirstChild(const flo_html_node_id parentID,
                                                const flo_html_node_id childID,
                                                flo_html_Dom *dom) {
    if ((dom->parentFirstChilds = flo_html_resizeArray(
             dom->parentFirstChilds, dom->parentFirstChildLen,
             &dom->parentFirstChildCap, sizeof(flo_html_ParentChild),
             FLO_HTML_PARENT_FIRST_CHILDS_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    flo_html_ParentChild *newParentFirstChild =
        &(dom->parentFirstChilds[dom->parentFirstChildLen]);
    newParentFirstChild->parentID = parentID;
    newParentFirstChild->childID = childID;
    dom->parentFirstChildLen++;
    return DOM_SUCCESS;
}

flo_html_DomStatus flo_html_addParentChild(const flo_html_node_id parentID,
                                           const flo_html_node_id childID,
                                           flo_html_Dom *dom) {
    if ((dom->parentChilds =
             flo_html_resizeArray(dom->parentChilds, dom->parentChildLen,
                         &dom->parentChildCap, sizeof(flo_html_ParentChild),
                         FLO_HTML_PARENT_CHILDS_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    flo_html_ParentChild *newParentChild =
        &(dom->parentChilds[dom->parentChildLen]);
    newParentChild->parentID = parentID;
    newParentChild->childID = childID;
    dom->parentChildLen++;
    return DOM_SUCCESS;
}

flo_html_DomStatus flo_html_addNextNode(const flo_html_node_id currentNodeID,
                                        const flo_html_node_id nextNodeID,
                                        flo_html_Dom *dom) {
    if ((dom->nextNodes =
             flo_html_resizeArray(dom->nextNodes, dom->nextNodeLen, &dom->nextNodeCap,
                         sizeof(flo_html_NextNode),
                         NEXT_FLO_HTML_NODES_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    flo_html_NextNode *newNextNode = &(dom->nextNodes[dom->nextNodeLen]);
    newNextNode->currentNodeID = currentNodeID;
    newNextNode->nextNodeID = nextNodeID;
    dom->nextNodeLen++;
    return DOM_SUCCESS;
}

flo_html_DomStatus flo_html_addBooleanProperty(const flo_html_node_id nodeID,
                                               const flo_html_element_id propID,
                                               flo_html_Dom *dom) {
    if ((dom->boolProps =
             flo_html_resizeArray(dom->boolProps, dom->boolPropsLen, &dom->boolPropsCap,
                         sizeof(flo_html_BooleanProperty),
                         BOOLEAN_PROPERTIES_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    flo_html_BooleanProperty *newBooleanProperty =
        &(dom->boolProps[dom->boolPropsLen]);
    newBooleanProperty->nodeID = nodeID;
    newBooleanProperty->propID = propID;
    dom->boolPropsLen++;
    return DOM_SUCCESS;
}

flo_html_DomStatus flo_html_addProperty(const flo_html_node_id nodeID,
                                        const flo_html_element_id keyID,
                                        const flo_html_element_id valueID,
                                        flo_html_Dom *dom) {
    if ((dom->props = flo_html_resizeArray(dom->props, dom->propsLen, &dom->propsCap,
                                  sizeof(flo_html_Property),
                                  PROPERTIES_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    flo_html_Property *newProperty = &(dom->props[dom->propsLen]);
    newProperty->nodeID = nodeID;
    newProperty->keyID = keyID;
    newProperty->valueID = valueID;
    dom->propsLen++;
    return DOM_SUCCESS;
}

const char *flo_html_getTag(const flo_html_indexID tagID,
                            const flo_html_Dom *dom,
                            const flo_html_TextStore *textStore) {
    flo_html_TagRegistration *tagRegistration = &dom->tagRegistry[tagID];
    return flo_html_getStringFromHashSet(&textStore->tags.set,
                                         &tagRegistration->hashElement);
}

void flo_html_getTagRegistration(flo_html_indexID tagID,
                                 const flo_html_Dom *dom,
                                 flo_html_TagRegistration **tagRegistration) {
    *tagRegistration = &dom->tagRegistry[tagID];
}

const char *flo_html_getBoolProp(const flo_html_indexID boolPropID,
                                 const flo_html_Dom *dom,
                                 const flo_html_TextStore *textStore) {
    flo_html_Registration registration =
        dom->boolPropRegistry.registry[boolPropID];
    return flo_html_getStringFromHashSet(&textStore->boolProps.set,
                                         &registration.hashElement);
}

const char *flo_html_getPropKey(const flo_html_indexID propKeyID,
                                const flo_html_Dom *dom,
                                const flo_html_TextStore *textStore) {
    flo_html_Registration registration =
        dom->propKeyRegistry.registry[propKeyID];
    return flo_html_getStringFromHashSet(&textStore->propKeys.set,
                                         &registration.hashElement);
}

const char *flo_html_getPropValue(const flo_html_indexID propValueID,
                                  const flo_html_Dom *dom,
                                  const flo_html_TextStore *textStore) {
    flo_html_Registration registration =
        dom->propValueRegistry.registry[propValueID];
    return flo_html_getStringFromHashSet(&textStore->propValues.set,
                                         &registration.hashElement);
}

flo_html_MergeResult flo_html_tryMerge(flo_html_Node *possibleMergeNode,
                                       flo_html_Node *replacingNode,
                                       flo_html_Dom *dom,
                                       flo_html_TextStore *textStore,
                                       bool isAppend) {
    if (possibleMergeNode->nodeType == NODE_TYPE_TEXT) {
        flo_html_ElementStatus elementStatus = flo_html_addTextToTextNode(
            possibleMergeNode, replacingNode->text, strlen(replacingNode->text),
            dom, textStore, isAppend);
        if (elementStatus != ELEMENT_CREATED) {
            FLO_HTML_PRINT_ERROR("Failed to merge new text node with up node!\n");
            return FAILED_MERGE;
        }
        return COMPLETED_MERGE;
    }
    return NO_MERGE;
}

flo_html_DomStatus
flo_html_connectOtherNodesToParent(const flo_html_node_id parentID,
                                   const flo_html_node_id lastAddedChild,
                                   flo_html_Dom *dom) {
    flo_html_node_id otherNewNodeID = flo_html_getNext(lastAddedChild, dom);
    while (otherNewNodeID > 0) {
        flo_html_DomStatus domStatus =
            flo_html_addParentChild(parentID, otherNewNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add new node ID as child!\n");
            return domStatus;
        }
        otherNewNodeID = flo_html_getNext(otherNewNodeID, dom);
    }

    return DOM_SUCCESS;
}
