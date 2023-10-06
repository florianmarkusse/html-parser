#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/reading/reading-util.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/element/element-status.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/utils/print/error.h"
#include <stdbool.h>
#include <string.h>

typedef enum {
    PROPERTY_TYPE_BOOL,
    PROPERTY_TYPE_KEY,
    PROPERTY_TYPE_VALUE,
    NUM_PROPERTY_TYPES
} PropertyType;

flo_html_ElementStatus
getCreatedPropIDFromString(const PropertyType propertyType,
                           const flo_html_String prop, flo_html_Dom *dom,
                           flo_html_StringRegistry *stringRegistry,
                           flo_html_element_id *propID) {
    flo_html_HashElement hashKey;

    flo_html_ElementStatus elementStatus =
        flo_html_elementToIndex(stringRegistry, prop, &hashKey, propID);
    if (elementStatus != ELEMENT_FOUND && elementStatus != ELEMENT_CREATED) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(elementStatus),
            "Failed to get prop ID");
        return elementStatus;
    }
    if (elementStatus == ELEMENT_CREATED) {
        flo_html_DomStatus domStatus = DOM_SUCCESS;

        switch (propertyType) {
        case PROPERTY_TYPE_BOOL: {
            domStatus =
                flo_html_addBoolPropRegistration(*propID, &hashKey, dom);
            break;
        }
        case PROPERTY_TYPE_KEY: {
            domStatus = flo_html_addPropKeyRegistration(*propID, &hashKey, dom);
            break;
        }
        case PROPERTY_TYPE_VALUE: {
            domStatus =
                flo_html_addPropValueRegistration(*propID, &hashKey, dom);
            break;
        }
        default: {
            FLO_HTML_PRINT_ERROR("Failed to specify prop type.\n");
            return ELEMENT_NOT_FOUND_OR_CREATED;
        }
        }

        if (domStatus != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add prop registration.\n");
            return ELEMENT_MEMORY;
        }
    }

    return ELEMENT_SUCCESS;
}

flo_html_ElementStatus
flo_html_addPropertyToNode(const flo_html_node_id nodeID,
                           const flo_html_String key,
                           const flo_html_String value, flo_html_Dom *dom,
                           flo_html_TextStore *textStore) {
    flo_html_ElementStatus result = ELEMENT_SUCCESS;

    flo_html_element_id keyID = 0;
    result = getCreatedPropIDFromString(PROPERTY_TYPE_KEY, key, dom,
                                        &textStore->propKeys, &keyID);
    if (result != ELEMENT_SUCCESS) {
        return result;
    }

    flo_html_element_id valueID = 0;
    result = getCreatedPropIDFromString(PROPERTY_TYPE_VALUE, value, dom,
                                        &textStore->propValues, &valueID);
    if (result != ELEMENT_SUCCESS) {
        return result;
    }

    if (flo_html_addProperty(nodeID, keyID, valueID, dom) != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add key-value property.\n");
        return ELEMENT_MEMORY;
    }

    return result;
}

flo_html_ElementStatus flo_html_addBooleanPropertyToNode(
    const flo_html_node_id nodeID, const flo_html_String boolProp,
    flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_ElementStatus result = ELEMENT_SUCCESS;

    flo_html_element_id boolPropID = 0;
    result = getCreatedPropIDFromString(PROPERTY_TYPE_BOOL, boolProp, dom,
                                        &textStore->boolProps, &boolPropID);
    if (result != ELEMENT_SUCCESS) {
        return result;
    }

    if (flo_html_addBooleanProperty(nodeID, boolPropID, dom) != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to add boolean property.\n");
        return ELEMENT_MEMORY;
    }

    return result;
}

flo_html_ElementStatus
flo_html_setPropertyValue(const flo_html_node_id nodeID,
                          const flo_html_String key,
                          const flo_html_String newValue, flo_html_Dom *dom,
                          flo_html_TextStore *textStore) {
    flo_html_element_id keyID = flo_html_getPropKeyID(key, textStore);
    if (keyID == 0) {
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    flo_html_Property *prop = flo_html_getProperty(nodeID, keyID, dom);
    if (prop == NULL) {
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    flo_html_element_id newValueID = 0;
    flo_html_ElementStatus result =
        getCreatedPropIDFromString(PROPERTY_TYPE_VALUE, newValue, dom,
                                   &textStore->propValues, &newValueID);
    if (result != ELEMENT_SUCCESS) {
        return result;
    }

    prop->valueID = newValueID;

    return ELEMENT_SUCCESS;
}

flo_html_DomStatus flo_html_setTextContent(const flo_html_node_id nodeID,
                                           const flo_html_String text,
                                           flo_html_Dom *dom,
                                           flo_html_TextStore *textStore) {
    flo_html_removeChildren(nodeID, dom);

    flo_html_node_id newNodeID = 0;
    flo_html_DomStatus domStatus =
        flo_html_parseTextElement(text, dom, textStore, &newNodeID);
    if (domStatus != DOM_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failed to parse text element!\n");
        return domStatus;
    }

    domStatus = flo_html_addParentFirstChild(nodeID, newNodeID, dom);
    if (domStatus != DOM_SUCCESS) {
        return domStatus;
    }

    domStatus = flo_html_addParentChild(nodeID, newNodeID, dom);

    return domStatus;
}

flo_html_ElementStatus flo_html_addTextToTextNode(flo_html_Node *node,
                                                  const flo_html_String text,
                                                  flo_html_Dom *dom,
                                                  flo_html_TextStore *textStore,
                                                  bool isAppend) {
    const flo_html_String prevText = node->text;
    const size_t mergedLen =
        prevText.len + text.len + 1; // Adding a whitespace in between.

    unsigned char buffer[mergedLen + 1];
    if (isAppend) {
        memcpy(buffer, prevText.buf, prevText.len);
        buffer[prevText.len] = ' ';
        memcpy(buffer + prevText.len + 1, text.buf, text.len);
    } else {
        memcpy(buffer, text.buf, text.len);
        buffer[text.len] = ' ';
        memcpy(buffer + text.len + 1, prevText.buf, prevText.len);
    }

    char *dataLocation = NULL;
    flo_html_ElementStatus elementStatus = flo_html_insertElement(
        &textStore->text, FLO_HTML_S_LEN(buffer, mergedLen), &dataLocation);
    if (elementStatus != ELEMENT_CREATED) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(elementStatus),
            "Failed to insert text");
        return elementStatus;
    }
    flo_html_setNodeText(node->nodeID, FLO_HTML_S_LEN(dataLocation, mergedLen),
                         dom);

    return elementStatus;
}

flo_html_DomStatus flo_html_setTagOnDocumentNode(
    const flo_html_String tag, const flo_html_node_id nodeID,
    const bool isPaired, flo_html_Dom *dom, flo_html_TextStore *textStore) {
    flo_html_DomStatus domStatus = DOM_SUCCESS;
    flo_html_HashElement hashElement;
    flo_html_indexID newTagID = 0;
    flo_html_ElementStatus indexStatus =
        flo_html_elementToIndex(&textStore->tags, tag, &hashElement, &newTagID);

    switch (indexStatus) {
    case ELEMENT_CREATED: {
        if ((domStatus = flo_html_addTagRegistration(
                 newTagID, isPaired, &hashElement, dom)) != DOM_SUCCESS) {
            FLO_HTML_PRINT_ERROR("Failed to add tag registration.\n");
            return domStatus;
        }
        // Intentional fall through!!!
    }
    case ELEMENT_FOUND: {
        flo_html_setNodeTagID(nodeID, newTagID, dom);
        break;
    }
    default: {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(indexStatus),
            "Failed to insert into new tag names!\n");
        return DOM_NO_ELEMENT;
    }
    }

    return domStatus;
}
