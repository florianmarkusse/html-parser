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

flo_html_index_id
getCreatedPropIDFromString(const PropertyType propertyType,
                           const flo_html_String prop, flo_html_Dom *dom,
                           flo_html_StringRegistry *stringRegistry,
                           flo_html_Arena *perm) {
    // TODO: make string hash support insert with getting entry back and whether
    // or not it was inserted.
    flo_html_ElementIndex result =
        flo_html_containsStringHashSet(&stringRegistry->set, prop);
    if (!result.entryIndex) {
        result.entryIndex =
            flo_html_insertStringHashSet(&stringRegistry->set, prop, perm);

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
                                const flo_html_String value,
                                flo_html_ParsedHTML parsed,
                                flo_html_Arena *perm) {
    flo_html_index_id keyID = getCreatedPropIDFromString(
        PROPERTY_TYPE_KEY, key, parsed.dom, &parsed.textStore->propKeys, perm);

    flo_html_index_id valueID =
        getCreatedPropIDFromString(PROPERTY_TYPE_VALUE, value, parsed.dom,
                                   &parsed.textStore->propValues, perm);

    *FLO_HTML_PUSH(&parsed.dom->props, perm) = (flo_html_Property){
        .nodeID = nodeID, .keyID = keyID, .valueID = valueID};
}

void flo_html_addBooleanPropertyToNode(const flo_html_node_id nodeID,
                                       const flo_html_String boolProp,
                                       flo_html_ParsedHTML parsed,
                                       flo_html_Arena *perm) {
    flo_html_index_id boolPropID =
        getCreatedPropIDFromString(PROPERTY_TYPE_BOOL, boolProp, parsed.dom,
                                   &parsed.textStore->boolProps, perm);

    *FLO_HTML_PUSH(&parsed.dom->boolProps, perm) =
        (flo_html_BooleanProperty){.nodeID = nodeID, .propID = boolPropID};
}

bool flo_html_setPropertyValue(const flo_html_node_id nodeID,
                               const flo_html_String key,
                               const flo_html_String newValue,
                               flo_html_ParsedHTML parsed,
                               flo_html_Arena *perm) {
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
                                   &parsed.textStore->propValues, perm);
    prop->valueID = newValueID;

    return true;
}

void flo_html_setTextContent(const flo_html_node_id nodeID,
                             const flo_html_String text,
                             flo_html_ParsedHTML parsed, flo_html_Arena *perm) {
    flo_html_removeChildren(nodeID, parsed.dom);

    flo_html_node_id newNodeID = flo_html_parseTextElement(text, parsed, perm);

    *FLO_HTML_PUSH(&parsed.dom->parentFirstChilds, perm) =
        (flo_html_ParentChild){.parentID = nodeID, .childID = newNodeID};
    *FLO_HTML_PUSH(&parsed.dom->parentChilds, perm) =
        (flo_html_ParentChild){.parentID = nodeID, .childID = newNodeID};
}

void flo_html_addTextToTextNode(flo_html_Node *node, const flo_html_String text,
                                flo_html_ParsedHTML parsed, bool isAppend,
                                flo_html_Arena *perm) {
    flo_html_String prevText = node->text;
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

    flo_html_setNodeText(node->nodeID, FLO_HTML_S_LEN(newText, mergedLen),
                         parsed.dom);
}

void flo_html_setTagOnDocumentNode(const flo_html_String tag,
                                   const flo_html_node_id nodeID,
                                   const bool isPaired,
                                   flo_html_ParsedHTML parsed,
                                   flo_html_Arena *perm) {
    // TODO: make string hash support insert with getting entry back and whether
    // or not it was inserted.
    flo_html_ElementIndex result =
        flo_html_containsStringHashSet(&parsed.textStore->tags.set, tag);
    if (!result.entryIndex) {
        result.entryIndex = flo_html_insertStringHashSet(
            &parsed.textStore->tags.set, tag, perm);
        *FLO_HTML_PUSH(&parsed.dom->tagRegistry, perm) =
            (flo_html_TagRegistration){.hashElement = result.hashElement,
                                       .isPaired = isPaired};
    }

    flo_html_setNodeTagID(nodeID, result.entryIndex, parsed.dom);
}
