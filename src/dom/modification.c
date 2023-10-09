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

    if (!elementIndex.wasPresent) {
        switch (propertyType) {
        case PROPERTY_TYPE_BOOL: {
            flo_html_addRegistration(&elementIndex.hashEntry.hashElement,
                                     &dom->boolPropRegistry);
            break;
        }
        case PROPERTY_TYPE_KEY: {
            flo_html_addRegistration(&elementIndex.hashEntry.hashElement,
                                     &dom->propKeyRegistry);
            break;
        }
        case PROPERTY_TYPE_VALUE: {
            flo_html_addRegistration(&elementIndex.hashEntry.hashElement,
                                     &dom->propValueRegistry);
            break;
        }
        }
    }

    return elementIndex.hashEntry.entryID;
}

void flo_html_addPropertyToNode(const flo_html_node_id nodeID,
                                const flo_html_String key,
                                const flo_html_String value, flo_html_Dom *dom,
                                flo_html_TextStore *textStore) {
    flo_html_index_id keyID = getCreatedPropIDFromString(
        PROPERTY_TYPE_KEY, key, dom, &textStore->propKeys);

    flo_html_index_id valueID = getCreatedPropIDFromString(
        PROPERTY_TYPE_VALUE, value, dom, &textStore->propValues);

    flo_html_addProperty(nodeID, keyID, valueID, dom);
}

void flo_html_addBooleanPropertyToNode(const flo_html_node_id nodeID,
                                       const flo_html_String boolProp,
                                       flo_html_Dom *dom,
                                       flo_html_TextStore *textStore) {
    flo_html_index_id boolPropID = getCreatedPropIDFromString(
        PROPERTY_TYPE_BOOL, boolProp, dom, &textStore->boolProps);

    flo_html_addBooleanProperty(nodeID, boolPropID, dom);
}

bool flo_html_setPropertyValue(const flo_html_node_id nodeID,
                               const flo_html_String key,
                               const flo_html_String newValue,
                               flo_html_Dom *dom,
                               flo_html_TextStore *textStore) {
    flo_html_index_id keyID =
        flo_html_getEntryID(key, &textStore->propKeys.set);
    if (keyID == 0) {
        FLO_HTML_PRINT_ERROR("Could not find key in stored prop keys\n");
        return false;
    }
    flo_html_Property *prop = flo_html_getProperty(nodeID, keyID, dom);
    if (prop == NULL) {
        FLO_HTML_PRINT_ERROR("Could not find key for given node\n");
        return false;
    }

    flo_html_index_id newValueID = getCreatedPropIDFromString(
        PROPERTY_TYPE_VALUE, newValue, dom, &textStore->propValues);
    prop->valueID = newValueID;

    return true;
}

void flo_html_setTextContent(const flo_html_node_id nodeID,
                             const flo_html_String text, flo_html_Dom *dom,
                             flo_html_TextStore *textStore) {
    flo_html_removeChildren(nodeID, dom);

    flo_html_node_id newNodeID =
        flo_html_parseTextElement(text, dom, textStore);
    flo_html_addParentFirstChild(nodeID, newNodeID, dom);
    flo_html_addParentChild(nodeID, newNodeID, dom);
}

void flo_html_addTextToTextNode(flo_html_Node *node, const flo_html_String text,
                                flo_html_Dom *dom,
                                flo_html_TextStore *textStore, bool isAppend) {
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
        FLO_HTML_S_LEN(buffer, mergedLen), &textStore->text);
    flo_html_setNodeText(node->nodeID, FLO_HTML_S_LEN(dataLocation, mergedLen),
                         dom);
}

void flo_html_setTagOnDocumentNode(const flo_html_String tag,
                                   const flo_html_node_id nodeID,
                                   const bool isPaired, flo_html_Dom *dom,
                                   flo_html_TextStore *textStore) {
    flo_html_ElementIndex elementIndex =
        flo_html_elementToIndex(&textStore->tags, tag);

    if (!elementIndex.wasPresent) {
        flo_html_addTagRegistration(elementIndex.hashEntry.entryID, isPaired,
                                    &elementIndex.hashEntry.hashElement, dom);
    }

    flo_html_setNodeTagID(nodeID, elementIndex.hashEntry.entryID, dom);
}