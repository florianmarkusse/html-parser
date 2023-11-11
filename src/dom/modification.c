#include "flo/html-parser/dom/modification.h"
#include "error.h"
#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/reading/reading-util.h"
#include "flo/html-parser/parser.h"
#include <stdbool.h>
#include <string.h>

typedef enum {
    PROPERTY_TYPE_BOOL,
    PROPERTY_TYPE_KEY,
    PROPERTY_TYPE_VALUE,
} PropertyType;

flo_html_index_id getCreatedPropIDFromString(PropertyType propertyType,
                                             flo_String prop, flo_html_Dom *dom,
                                             flo_StringHashSet *set,
                                             flo_Arena *perm) {
    flo_StringInsert result = flo_insertStringHashSet(set, prop, perm);
    if (result.entryIndex > FLO_HTML_MAX_NODE_ID) {
        __builtin_longjmp(perm->jmp_buf, 1);
    }
    if (result.wasInserted) {
        switch (propertyType) {
        case PROPERTY_TYPE_BOOL: {
            *FLO_PUSH(&dom->boolPropRegistry, perm) = prop;
            break;
        }
        case PROPERTY_TYPE_KEY: {
            *FLO_PUSH(&dom->propKeyRegistry, perm) = prop;
            break;
        }
        case PROPERTY_TYPE_VALUE: {
            *FLO_PUSH(&dom->propValueRegistry, perm) = prop;
            break;
        }
        }
    }

    return (flo_html_index_id)result.entryIndex;
}

void flo_html_addPropertyToNode(flo_html_node_id nodeID, flo_String key,
                                flo_String value, flo_html_Dom *dom,
                                flo_Arena *perm) {
    flo_html_index_id keyID = getCreatedPropIDFromString(
        PROPERTY_TYPE_KEY, key, dom, &dom->propKeys, perm);

    flo_html_index_id valueID = getCreatedPropIDFromString(
        PROPERTY_TYPE_VALUE, value, dom, &dom->propValues, perm);

    *FLO_PUSH(&dom->props, perm) = (flo_html_Property){
        .nodeID = nodeID, .keyID = keyID, .valueID = valueID};
}

void flo_html_addBooleanPropertyToNode(flo_html_node_id nodeID,
                                       flo_String boolProp, flo_html_Dom *dom,
                                       flo_Arena *perm) {
    flo_html_index_id boolPropID = getCreatedPropIDFromString(
        PROPERTY_TYPE_BOOL, boolProp, dom, &dom->boolPropsSet, perm);

    *FLO_PUSH(&dom->boolProps, perm) =
        (flo_html_BooleanProperty){.nodeID = nodeID, .propID = boolPropID};
}

bool flo_html_setPropertyValue(flo_html_node_id nodeID, flo_String key,
                               flo_String newValue, flo_html_Dom *dom,
                               flo_Arena *perm) {
    flo_html_index_id keyID =
        (flo_html_index_id)flo_containsStringHashSet(&dom->propKeys, key);
    if (keyID == 0) {
        FLO_PRINT_ERROR("Could not find key in stored prop keys\n");
        return false;
    }
    flo_html_Property *prop = flo_html_getProperty(nodeID, keyID, dom);
    if (prop == NULL) {
        FLO_PRINT_ERROR("Could not find key for given node\n");
        return false;
    }

    flo_html_index_id createdValueID = getCreatedPropIDFromString(
        PROPERTY_TYPE_VALUE, newValue, dom, &dom->propValues, perm);
    prop->valueID = createdValueID;

    return true;
}

void flo_html_setTextContent(flo_html_node_id nodeID, flo_String text,
                             flo_html_Dom *dom, flo_Arena *perm) {
    flo_html_removeChildren(nodeID, dom);

    flo_html_node_id newNodeID = flo_html_parseTextElement(text, dom, perm);

    *FLO_PUSH(&dom->parentFirstChilds, perm) =
        (flo_html_ParentChild){.parentID = nodeID, .childID = newNodeID};
    *FLO_PUSH(&dom->parentChilds, perm) =
        (flo_html_ParentChild){.parentID = nodeID, .childID = newNodeID};
}

void flo_html_addTextToTextNode(flo_html_node_id nodeID, flo_String text,
                                flo_html_Dom *dom, bool isAppend,
                                flo_Arena *perm) {
    flo_String prevText = dom->nodes.buf[nodeID].text;
    ptrdiff_t mergedLen =
        prevText.len + text.len + 1; // Adding a whitespace in between.

    unsigned char *newText = FLO_NEW(perm, unsigned char, mergedLen);

    if (isAppend) {
        memcpy(newText, prevText.buf, prevText.len);
        newText[prevText.len] = ' ';
        memcpy(newText + prevText.len + 1, text.buf, text.len);
    } else {
        memcpy(newText, text.buf, text.len);
        newText[text.len] = ' ';
        memcpy(newText + text.len + 1, prevText.buf, prevText.len);
    }

    dom->nodes.buf[nodeID].text = FLO_STRING_LEN(newText, mergedLen);
}

void flo_html_setTagOnDocumentNode(flo_String tag, flo_html_node_id nodeID,
                                   bool isPaired, flo_html_Dom *dom,
                                   flo_Arena *perm) {
    flo_StringInsert result = flo_insertStringHashSet(&dom->tags, tag, perm);
    if (result.entryIndex > FLO_HTML_MAX_INDEX_ID) {
        __builtin_longjmp(perm->jmp_buf, 1);
    }
    if (result.wasInserted) {
        *FLO_PUSH(&dom->tagRegistry, perm) =
            (flo_html_TagRegistration){.tag = tag, .isPaired = isPaired};
    }

    dom->nodes.buf[nodeID].tagID = (flo_html_index_id)result.entryIndex;
}
