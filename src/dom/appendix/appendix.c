
#include <string.h>

#include "flo/html-parser/dom/appendix/appendix.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/type/node/parent-child.h"
#include "flo/html-parser/utils/print/error.h"

DomStatus addTagToNodeID(const char *tagStart, const size_t elementStart,
                         const node_id nodeID, const bool isPaired, Dom *dom,
                         DataContainer *dataContainer) {
    DomStatus domStatus = DOM_SUCCESS;
    HashElement hashElement;
    indexID newTagID = 0;
    ElementStatus indexStatus = elementToIndex(
        &dataContainer->tags, tagStart, elementStart, &hashElement, &newTagID);

    switch (indexStatus) {
    case ELEMENT_CREATED: {
        if ((domStatus = addTagRegistration(newTagID, isPaired, &hashElement,
                                            dom)) != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add tag registration.\n");
            return domStatus;
        }
        // Intentional fall through!!!
    }
    case ELEMENT_FOUND: {
        setNodeTagID(nodeID, newTagID, dom);
        break;
    }
    default: {
        ERROR_WITH_CODE_ONLY(elementStatusToString(indexStatus),
                             "Failed to insert into new tag names!\n");
        return DOM_NO_ELEMENT;
    }
    }

    return domStatus;
}

static DomStatus updateReferences(const node_id parentID,
                                  const node_id newNodeID, Dom *dom) {
    DomStatus domStatus = DOM_SUCCESS;
    ParentChild *firstChild = getFirstChildNode(parentID, dom);
    if (firstChild == NULL) {
        domStatus = addParentFirstChild(parentID, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID as first child!\n");
            return domStatus;
        }

        domStatus = addParentChild(parentID, newNodeID, dom);
        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add new node ID as child!\n");
            return domStatus;
        }

        return domStatus;
    }

    node_id nextNode = firstChild->childID;
    node_id nextNextNode = getNext(nextNode, dom);
    while (nextNextNode > 0) {
        nextNode = nextNextNode;
        nextNextNode = getNext(nextNode, dom);
    }

    domStatus = addNextNode(nextNode, newNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add new node ID in next nodes!\n");
        return domStatus;
    }

    domStatus = addParentChild(parentID, newNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add new node ID as child!\n");
        return domStatus;
    }

    return domStatus;
}

DomStatus appendDocumentNode(const node_id parentID,
                             const AppendDocumentNode *docNode, Dom *dom,
                             DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus = createNode(&newNodeID, NODE_TYPE_DOCUMENT, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to retrieve a new node ID!\n");
        return domStatus;
    }

    domStatus = addTagToNodeID(docNode->tag, strlen(docNode->tag), newNodeID,
                               docNode->isPaired, dom, dataContainer);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add tag to new node ID!\n");
        return domStatus;
    }

    for (size_t i = 0; i < docNode->boolPropsLen; i++) {
        const char *boolProp = docNode->boolProps[i];
        ElementStatus elementStatus = addBooleanPropertyToNodeString(
            newNodeID, boolProp, dom, dataContainer);
        if (elementStatus != ELEMENT_SUCCESS) {
            PRINT_ERROR("Failed to boolean property to new node ID!\n");
            return DOM_NO_ELEMENT;
        }
    }

    for (size_t i = 0; i < docNode->propsLen; i++) {
        const char *keyProp = docNode->keyProps[i];
        const char *valueProp = docNode->valueProps[i];
        ElementStatus elementStatus = addPropertyToNodeStrings(
            newNodeID, keyProp, valueProp, dom, dataContainer);
        if (elementStatus != ELEMENT_SUCCESS) {
            PRINT_ERROR("Failed to property to new node ID!\n");
            return DOM_NO_ELEMENT;
        }
    }

    return updateReferences(parentID, newNodeID, dom);
}

DomStatus appendTextNode(const node_id parentID, const char *text, Dom *dom,
                         DataContainer *dataContainer) {
    node_id newNodeID = 0;
    DomStatus domStatus = createNode(&newNodeID, NODE_TYPE_TEXT, dom);
    if (domStatus != DOM_SUCCESS) {
        PRINT_ERROR("Failed to retrieve a new node ID!\n");
        return domStatus;
    }

    char *dataLocation = NULL;
    ElementStatus elementStatus =
        insertElement(&dataContainer->text, text, strlen(text), &dataLocation);
    if (elementStatus != ELEMENT_CREATED) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(elementStatus),
                             "Failed to index text!\n");
        return DOM_NO_ELEMENT;
    }

    setNodeText(newNodeID, dataLocation, dom);

    return updateReferences(parentID, newNodeID, dom);
}
