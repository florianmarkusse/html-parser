#include <stdint.h>
#include <stdlib.h>

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/utils/memory/memory.h"
#include "flo/html-parser/utils/print/error.h"

void initBasicRegistry(BasicRegistry *basicRegistry,
                       const Registration *initRegistration,
                       const size_t pageSize, const size_t elementsPerPage) {
    basicRegistry->registry = malloc(pageSize);
    basicRegistry->registry[0] = *initRegistration;
    basicRegistry->len =
        1; // Start at 1 so we don't need to do tagRegistry[x - 1]
    basicRegistry->cap = elementsPerPage;
}

DomStatus createDom(const char *htmlString, Dom *dom,
                    DataContainer *dataContainer) {
    dom->firstNodeID = 0;

    TagRegistration initTag;
    initTag.tagID = 0;
    initTag.isPaired = false;
    initTag.hashElement.hash = 0;
    initTag.hashElement.offset = 0;

    Registration initRegistration;
    initRegistration.indexID = 0;
    initRegistration.hashElement.hash = 0;
    initRegistration.hashElement.offset = 0;

    dom->tagRegistry = malloc(TAG_REGISTRY_PAGE_SIZE);
    dom->tagRegistry[0] = initTag;
    dom->tagRegistryLen =
        1; // Start at 1 so we don't need to do tagRegistry[x - 1]
    dom->tagRegistryCap = TAG_REGISTRATIONS_PER_PAGE;

    initBasicRegistry(&dom->boolPropRegistry, &initRegistration,
                      BOOL_PROP_REGISTRY_PAGE_SIZE,
                      BOOL_PROP_REGISTRATIONS_PER_PAGE);
    initBasicRegistry(&dom->propKeyRegistry, &initRegistration,
                      PROP_KEY_REGISTRY_PAGE_SIZE,
                      PROP_KEY_REGISTRATIONS_PER_PAGE);
    initBasicRegistry(&dom->propValueRegistry, &initRegistration,
                      PROP_VALUE_REGISTRY_PAGE_SIZE,
                      PROP_VALUE_REGISTRATIONS_PER_PAGE);
    initBasicRegistry(&dom->textRegistry, &initRegistration,
                      TEXT_REGISTRY_PAGE_SIZE, TEXT_REGISTRATIONS_PER_PAGE);

    dom->nodes = malloc(NODES_PAGE_SIZE);
    Node errorNode;
    errorNode.nodeID = ERROR_NODE_ID;
    errorNode.nodeType = NODE_TYPE_ERROR;
    errorNode.tagID = ERROR_NODE_ID;
    dom->nodes[0] = errorNode;
    dom->nodeLen = 1; // We start at 1 because 0 is used as error id, and
                      // otherwise we have to do [nodeID - 1] every time.
    dom->nodeCap = NODES_PER_PAGE;

    dom->parentFirstChilds = malloc(PARENT_FIRST_CHILDS_PAGE_SIZE);
    dom->parentFirstChildLen = 0;
    dom->parentFirstChildCap = PARENT_FIRST_CHILDS_PER_PAGE;

    dom->parentChilds = malloc(PARENT_CHILDS_PAGE_SIZE);
    dom->parentChildLen = 0;
    dom->parentChildCap = PARENT_CHILDS_PER_PAGE;

    dom->nextNodes = malloc(NEXT_NODES_PAGE_SIZE);
    dom->nextNodeLen = 0;
    dom->nextNodeCap = NEXT_NODES_PER_PAGE;

    dom->boolProps = malloc(BOOLEAN_PROPERTIES_PAGE_SIZE);
    dom->boolPropsLen = 0;
    dom->boolPropsCap = BOOLEAN_PROPERTIES_PER_PAGE;

    dom->props = malloc(PROPERTIES_PAGE_SIZE);
    dom->propsLen = 0;
    dom->propsCap = PROPERTIES_PER_PAGE;

    dom->text = malloc(TEXT_NODES_PAGE_SIZE);
    dom->textLen = 0;
    dom->textCap = TEXT_NODES_PER_PAGE;

    if (dom->nodes == NULL || dom->tagRegistry == NULL ||
        dom->boolPropRegistry.registry == NULL ||
        dom->propKeyRegistry.registry == NULL ||
        dom->propValueRegistry.registry == NULL ||
        dom->textRegistry.registry == NULL || dom->parentFirstChilds == NULL ||
        dom->parentChilds == NULL || dom->nextNodes == NULL ||
        dom->boolProps == NULL || dom->props == NULL || dom->text == NULL) {
        PRINT_ERROR("Failed to allocate memory for nodes.\n");
        destroyDom(dom);
        return DOM_ERROR_MEMORY;
    }

    DomStatus domumentStatus = parse(htmlString, dom, dataContainer);
    if (domumentStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to parse domument.\n");
    }
    return domumentStatus;
}

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
DomStatus setNodeTagID(const node_id nodeID, const indexID tagID, Dom *dom) {
    Node *createdNode = &(dom->nodes[nodeID]);
    createdNode->tagID = tagID;

    return DOM_SUCCESS;
}

DomStatus addParentFirstChild(const node_id parentID, const node_id childID,
                              Dom *dom) {
    if ((dom->parentFirstChilds =
             resizeArray(dom->parentFirstChilds, dom->parentFirstChildLen,
                         &dom->parentFirstChildCap, sizeof(ParentFirstChild),
                         PARENT_FIRST_CHILDS_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    ParentFirstChild *newParentFirstChild =
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

DomStatus addTextNode(const node_id nodeID, const element_id textID, Dom *dom) {
    if ((dom->text = resizeArray(dom->text, dom->textLen, &dom->textCap,
                                 sizeof(TextNode), TEXT_NODES_PER_PAGE)) ==
        NULL) {
        return DOM_ERROR_MEMORY;
    }

    TextNode *newTextNode = &(dom->text[dom->textLen]);
    newTextNode->nodeID = nodeID;
    newTextNode->textID = textID;
    dom->textLen++;
    return DOM_SUCCESS;
}

DomStatus replaceTextNode(const node_id nodeID, element_id newTextID,
                          Dom *dom) {
    for (size_t i = 0; i < dom->textLen; i++) {
        if (dom->text[i].nodeID == nodeID) {
            dom->text[i].textID = newTextID;
            break;
        }
    }

    return DOM_SUCCESS;
}

void destroyDom(Dom *dom) {
    FREE_TO_NULL(dom->nodes);
    FREE_TO_NULL(dom->tagRegistry);
    FREE_TO_NULL(dom->boolPropRegistry.registry);
    FREE_TO_NULL(dom->propKeyRegistry.registry);
    FREE_TO_NULL(dom->propValueRegistry.registry);
    FREE_TO_NULL(dom->textRegistry.registry);
    FREE_TO_NULL(dom->parentFirstChilds);
    FREE_TO_NULL(dom->parentChilds);
    FREE_TO_NULL(dom->nextNodes);
    FREE_TO_NULL(dom->boolProps);
    FREE_TO_NULL(dom->props);
    FREE_TO_NULL(dom->text);
}
