#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/reading/reading-util.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/type/element/element-status.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/utils/print/error.h"
#include <string.h>

ElementStatus getCreatedPropIDFromString(const PropertyType propertyType,
                                         const char *buffer,
                                         const size_t propLen, Dom *dom,
                                         StringRegistry *stringRegistry,
                                         element_id *propID) {
    HashElement hashKey;

    ElementStatus elementStatus =
        elementToIndex(stringRegistry, buffer, propLen, &hashKey, propID);
    if (elementStatus != ELEMENT_FOUND && elementStatus != ELEMENT_CREATED) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(elementStatus),
                             "Failed to get prop ID");
        return elementStatus;
    }
    if (elementStatus == ELEMENT_CREATED) {
        DomStatus domStatus = DOM_SUCCESS;

        switch (propertyType) {
        case PROPERTY_TYPE_BOOL: {
            domStatus = addBoolPropRegistration(*propID, &hashKey, dom);
            break;
        }
        case PROPERTY_TYPE_KEY: {
            domStatus = addPropKeyRegistration(*propID, &hashKey, dom);
            break;
        }
        case PROPERTY_TYPE_VALUE: {
            domStatus = addPropValueRegistration(*propID, &hashKey, dom);
            break;
        }
        default: {
            PRINT_ERROR("Failed to specify prop type.\n");
            return ELEMENT_NOT_FOUND_OR_CREATED;
        }
        }

        if (domStatus != DOM_SUCCESS) {
            PRINT_ERROR("Failed to add prop registration.\n");
            return ELEMENT_MEMORY;
        }
    }

    return ELEMENT_SUCCESS;
}

ElementStatus
addPropertyToNodeStringsWithLength(const node_id nodeID, const char *keyBuffer,
                                   const size_t keyLen, const char *valueBuffer,
                                   const size_t valueLen, Dom *dom,
                                   DataContainer *dataContainer) {
    ElementStatus result = ELEMENT_SUCCESS;

    element_id keyID = 0;
    result = getCreatedPropIDFromString(PROPERTY_TYPE_KEY, keyBuffer, keyLen,
                                        dom, &dataContainer->propKeys, &keyID);
    if (result != ELEMENT_SUCCESS) {
        return result;
    }

    element_id valueID = 0;
    result =
        getCreatedPropIDFromString(PROPERTY_TYPE_VALUE, valueBuffer, valueLen,
                                   dom, &dataContainer->propValues, &valueID);
    if (result != ELEMENT_SUCCESS) {
        return result;
    }

    if (addProperty(nodeID, keyID, valueID, dom) != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add key-value property.\n");
        return ELEMENT_MEMORY;
    }

    return result;
}

ElementStatus addPropertyToNodeStrings(const node_id nodeID,
                                       const char *keyBuffer,
                                       const char *valueBuffer, Dom *dom,
                                       DataContainer *dataContainer) {
    return addPropertyToNodeStringsWithLength(
        nodeID, keyBuffer, strlen(keyBuffer), valueBuffer, strlen(valueBuffer),
        dom, dataContainer);
}

ElementStatus addBooleanPropertyToNodeStringWithLength(
    const node_id nodeID, const char *boolPropBuffer, const size_t boolPropLen,
    Dom *dom, DataContainer *dataContainer) {
    ElementStatus result = ELEMENT_SUCCESS;

    element_id boolPropID = 0;
    result = getCreatedPropIDFromString(PROPERTY_TYPE_BOOL, boolPropBuffer,
                                        boolPropLen, dom,
                                        &dataContainer->boolProps, &boolPropID);
    if (result != ELEMENT_SUCCESS) {
        return result;
    }

    if (addBooleanProperty(nodeID, boolPropID, dom) != DOM_SUCCESS) {
        PRINT_ERROR("Failed to add boolean property.\n");
        return ELEMENT_MEMORY;
    }

    return result;
}

ElementStatus addBooleanPropertyToNodeString(const node_id nodeID,
                                             const char *boolPropBuffer,
                                             Dom *dom,
                                             DataContainer *dataContainer) {
    return addBooleanPropertyToNodeStringWithLength(
        nodeID, boolPropBuffer, strlen(boolPropBuffer), dom, dataContainer);
}

ElementStatus setPropertyValue(const node_id nodeID, const char *key,
                               const char *newValue, Dom *dom,
                               DataContainer *dataContainer) {
    element_id keyID = getPropKeyID(key, dataContainer);
    if (keyID == 0) {
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    Property *prop = getProperty(nodeID, keyID, dom);
    if (prop == NULL) {
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    element_id newValueID = 0;
    ElementStatus result = getCreatedPropIDFromString(
        PROPERTY_TYPE_VALUE, newValue, strlen(newValue), dom,
        &dataContainer->propValues, &newValueID);
    if (result != ELEMENT_SUCCESS) {
        return result;
    }

    prop->valueID = newValueID;

    return ELEMENT_SUCCESS;
}

ElementStatus appendTextToTextNode(Node *node, const char *textStart,
                                   const size_t textLen, Dom *dom,
                                   DataContainer *dataContainer) {
    const char *prevText = node->text;
    const size_t mergedLen =
        strlen(prevText) + textLen + 2; // Adding a whitespace in between.

    char buffer[mergedLen];
    strcpy(buffer, prevText);
    strcat(buffer, " ");
    strncat(buffer, textStart, textLen);
    buffer[mergedLen - 1] = '\0';

    char *dataLocation = NULL;
    ElementStatus elementStatus =
        insertElement(&dataContainer->text, buffer, mergedLen, &dataLocation);
    if (elementStatus != ELEMENT_CREATED) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(elementStatus),
                             "Failed to insert text");
        return elementStatus;
    }
    setNodeText(node->nodeID, dataLocation, dom);

    return elementStatus;
}

DomStatus setTagOnDocumentNode(const char *tagStart, const size_t elementStart,
                               const node_id nodeID, const bool isPaired,
                               Dom *dom, DataContainer *dataContainer) {
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
