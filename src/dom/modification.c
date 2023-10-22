#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/reading/reading-util.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/error.h"
#include <stdbool.h>
#include <string.h>

typedef enum {
    PROPERTY_TYPE_BOOL,
    PROPERTY_TYPE_KEY,
    PROPERTY_TYPE_VALUE,
} PropertyType;

flo_html_index_id getCreatedPropIDFromString(const PropertyType propertyType,
                                             const flo_html_String prop,
                                             flo_html_Dom *dom,
                                             flo_html_StringHashSet *set,
                                             flo_html_Arena *perm) {
    // TODO: make string hash support insert with getting entry back and whether
    // or not it was inserted.
    flo_html_Contains result = flo_html_containsStringHashSet(set, prop);
    if (!result.entryIndex) {
        result.entryIndex = flo_html_insertStringHashSet(set, prop, perm);

        switch (propertyType) {
        case PROPERTY_TYPE_BOOL: {
            *FLO_HTML_PUSH(&dom->boolPropRegistry, perm) = result.hashElement;
            break;
        }
        case PROPERTY_TYPE_KEY: {
            *FLO_HTML_PUSH(&dom->propKeyRegistry, perm) = result.hashElement;
            break;
        }
        case PROPERTY_TYPE_VALUE: {
            *FLO_HTML_PUSH(&dom->propValueRegistry, perm) = result.hashElement;
            break;
        }
        }
    }

    return result.entryIndex;
}

void flo_html_addPropertyToNode(const flo_html_node_id nodeID,
                                const flo_html_String key,
                                const flo_html_String value, flo_html_Dom *dom,
                                flo_html_Arena *perm) {
    flo_html_index_id keyID = getCreatedPropIDFromString(
        PROPERTY_TYPE_KEY, key, dom, &dom->propKeys, perm);

    flo_html_index_id valueID = getCreatedPropIDFromString(
        PROPERTY_TYPE_VALUE, value, dom, &dom->propValues, perm);

    *FLO_HTML_PUSH(&dom->props, perm) = (flo_html_Property){
        .nodeID = nodeID, .keyID = keyID, .valueID = valueID};
}

void flo_html_addBooleanPropertyToNode(const flo_html_node_id nodeID,
                                       const flo_html_String boolProp,
                                       flo_html_Dom *dom,
                                       flo_html_Arena *perm) {
    flo_html_index_id boolPropID = getCreatedPropIDFromString(
        PROPERTY_TYPE_BOOL, boolProp, dom, &dom->boolPropsSet, perm);

    *FLO_HTML_PUSH(&dom->boolProps, perm) =
        (flo_html_BooleanProperty){.nodeID = nodeID, .propID = boolPropID};
}

bool flo_html_setPropertyValue(const flo_html_node_id nodeID,
                               const flo_html_String key,
                               const flo_html_String newValue,
                               flo_html_Dom *dom, flo_html_Arena *perm) {
    flo_html_index_id keyID =
        flo_html_containsStringHashSet(&dom->propKeys, key).entryIndex;
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
        PROPERTY_TYPE_VALUE, newValue, dom, &dom->propValues, perm);
    prop->valueID = newValueID;

    return true;
}

void flo_html_setTextContent(const flo_html_node_id nodeID,
                             const flo_html_String text, flo_html_Dom *dom,
                             flo_html_Arena *perm) {
    flo_html_removeChildren(nodeID, dom);

    flo_html_node_id newNodeID = flo_html_parseTextElement(text, dom, perm);

    *FLO_HTML_PUSH(&dom->parentFirstChilds, perm) =
        (flo_html_ParentChild){.parentID = nodeID, .childID = newNodeID};
    *FLO_HTML_PUSH(&dom->parentChilds, perm) =
        (flo_html_ParentChild){.parentID = nodeID, .childID = newNodeID};
}

void flo_html_addTextToTextNode(flo_html_node_id nodeID,
                                const flo_html_String text, flo_html_Dom *dom,
                                bool isAppend, flo_html_Arena *perm) {
    flo_html_String prevText = dom->nodes.buf[nodeID].text;
    ptrdiff_t mergedLen =
        prevText.len + text.len + 1; // Adding a whitespace in between.

    unsigned char *newText = FLO_HTML_NEW(perm, unsigned char, mergedLen);

    if (isAppend) {
        memcpy(newText, prevText.buf, prevText.len);
        newText[prevText.len] = ' ';
        memcpy(newText + prevText.len + 1, text.buf, text.len);
    } else {
        memcpy(newText, text.buf, text.len);
        newText[text.len] = ' ';
        memcpy(newText + text.len + 1, prevText.buf, prevText.len);
    }

    dom->nodes.buf[nodeID].text = FLO_HTML_S_LEN(newText, mergedLen);
}

void flo_html_setTagOnDocumentNode(const flo_html_String tag,
                                   const flo_html_node_id nodeID,
                                   const bool isPaired, flo_html_Dom *dom,
                                   flo_html_Arena *perm) {
    // TODO: make string hash support insert with getting entry back and whether
    // or not it was inserted.
    flo_html_Contains result = flo_html_containsStringHashSet(&dom->tags, tag);
    if (!result.entryIndex) {
        result.entryIndex = flo_html_insertStringHashSet(&dom->tags, tag, perm);
        *FLO_HTML_PUSH(&dom->tagRegistry, perm) = (flo_html_TagRegistration){
            .hashElement = result.hashElement, .isPaired = isPaired};
    }

    dom->nodes.buf[nodeID].tagID = result.entryIndex;
}
