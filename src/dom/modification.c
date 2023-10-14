#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/reading/reading-util.h"
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/type/element/element-status.h"
#include "flo/html-parser/util/error.h"
#include <stdbool.h>
#include <string.h>

typedef enum {
    PROPERTY_TYPE_BOOL,
    PROPERTY_TYPE_KEY,
    PROPERTY_TYPE_VALUE,
} PropertyType;

flo_html_index_id
getCreatedPropIDFromString(const PropertyType propertyType,
                           const flo_html_String prop, flo_html_Dom *dom,
                           flo_html_StringRegistry *stringRegistry) {
    flo_html_ElementIndex elementIndex =
        flo_html_elementToIndex(stringRegistry, prop);

    if (!elementIndex.entryIndex) {
        switch (propertyType) {
        case PROPERTY_TYPE_BOOL: {
            flo_html_addRegistration(&elementIndex.hashElement,
                                     &dom->boolPropRegistry);
            break;
        }
        case PROPERTY_TYPE_KEY: {
            flo_html_addRegistration(&elementIndex.hashElement,
                                     &dom->propKeyRegistry);
            break;
        }
        case PROPERTY_TYPE_VALUE: {
            flo_html_addRegistration(&elementIndex.hashElement,
                                     &dom->propValueRegistry);
            break;
        }
        }
    }

    return elementIndex.entryIndex;
}

void flo_html_addPropertyToNode(const flo_html_node_id nodeID,
                                const flo_html_String key,
                                const flo_html_String value,
                                flo_html_ParsedHTML parsed) {
    flo_html_index_id keyID = getCreatedPropIDFromString(
        PROPERTY_TYPE_KEY, key, parsed.dom, &parsed.textStore->propKeys);

    flo_html_index_id valueID = getCreatedPropIDFromString(
        PROPERTY_TYPE_VALUE, value, parsed.dom, &parsed.textStore->propValues);

    flo_html_addProperty(nodeID, keyID, valueID, parsed.dom);
}

void flo_html_addBooleanPropertyToNode(const flo_html_node_id nodeID,
                                       const flo_html_String boolProp,
                                       flo_html_ParsedHTML parsed) {
    flo_html_index_id boolPropID = getCreatedPropIDFromString(
        PROPERTY_TYPE_BOOL, boolProp, parsed.dom, &parsed.textStore->boolProps);

    flo_html_addBooleanProperty(nodeID, boolPropID, parsed.dom);
}

bool flo_html_setPropertyValue(const flo_html_node_id nodeID,
                               const flo_html_String key,
                               const flo_html_String newValue,
                               flo_html_ParsedHTML parsed) {
    flo_html_index_id keyID =
        flo_html_containsStringHashSet(&parsed.textStore->propKeys.set, key)
            .entryIndex;
    if (keyID == 0) {
        FLO_HTML_PRINT_ERROR("Could not find key in stored prop keys\n");
        return false;
    }
    flo_html_Property *prop = flo_html_getProperty(nodeID, keyID, parsed.dom);
    if (prop == NULL) {
        FLO_HTML_PRINT_ERROR("Could not find key for given node\n");
        return false;
    }

    flo_html_index_id newValueID =
        getCreatedPropIDFromString(PROPERTY_TYPE_VALUE, newValue, parsed.dom,
                                   &parsed.textStore->propValues);
    prop->valueID = newValueID;

    return true;
}

void flo_html_setTextContent(const flo_html_node_id nodeID,
                             const flo_html_String text,
                             flo_html_ParsedHTML parsed) {
    flo_html_removeChildren(nodeID, parsed.dom);

    flo_html_node_id newNodeID = flo_html_parseTextElement(text, parsed);
    flo_html_addParentFirstChild(nodeID, newNodeID, parsed.dom);
    flo_html_addParentChild(nodeID, newNodeID, parsed.dom);
}

void flo_html_addTextToTextNode(flo_html_Node *node, const flo_html_String text,
                                flo_html_ParsedHTML parsed, bool isAppend) {
    const flo_html_String prevText = node->text;
    const ptrdiff_t mergedLen =
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

    unsigned char *dataLocation = flo_html_insertIntoPage(
        FLO_HTML_S_LEN(buffer, mergedLen), &parsed.textStore->text);
    flo_html_setNodeText(node->nodeID, FLO_HTML_S_LEN(dataLocation, mergedLen),
                         parsed.dom);
}

void flo_html_setTagOnDocumentNode(const flo_html_String tag,
                                   const flo_html_node_id nodeID,
                                   const bool isPaired,
                                   flo_html_ParsedHTML parsed) {
    flo_html_ElementIndex elementIndex =
        flo_html_elementToIndex(&parsed.textStore->tags, tag);

    if (elementIndex.entryIndex > 0) {
        flo_html_addTagRegistration(isPaired, &elementIndex.hashElement,
                                    parsed.dom);
    }

    flo_html_setNodeTagID(nodeID, elementIndex.entryIndex, parsed.dom);
}
