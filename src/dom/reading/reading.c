#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/reading/reading.h"
#include "flo/html-parser/dom/traversal.h"
#include "memory/arena.h"

bool flo_html_hasBoolProp(flo_html_node_id nodeID, flo_String boolProp,
                          flo_html_Dom *dom) {
    flo_html_index_id boolPropID = (flo_html_index_id)flo_containsStringHashSet(
        &dom->boolPropsSet, boolProp);
    if (boolPropID == 0) {
        return false;
    }

    for (ptrdiff_t i = 0; i < dom->boolProps.len; i++) {
        flo_html_BooleanProperty *booleanProperty = &dom->boolProps.buf[i];
        if (booleanProperty->nodeID == nodeID &&
            booleanProperty->propID == boolPropID) {
            return true;
        }
    }
    return false;
}

bool flo_html_hasPropKey(flo_html_node_id nodeID, flo_String propKey,
                         flo_html_Dom *dom) {
    flo_html_index_id propKeyID =
        (flo_html_index_id)flo_containsStringHashSet(&dom->propKeys, propKey);
    if (propKeyID == 0) {
        return false;
    }

    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        flo_html_Property *property = &dom->props.buf[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID) {
            return true;
        }
    }
    return false;
}

bool flo_html_hasPropValue(flo_html_node_id nodeID, flo_String propValue,
                           flo_html_Dom *dom) {
    flo_html_index_id propValueID =
        (flo_html_index_id)flo_containsStringHashSet(&dom->propValues,
                                                     propValue);
    if (propValueID == 0) {
        return false;
    }

    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        flo_html_Property *property = &dom->props.buf[i];
        if (property->nodeID == nodeID && property->valueID == propValueID) {
            return true;
        }
    }
    return false;
}

bool flo_html_hasProperty(flo_html_node_id nodeID, flo_String propKey,
                          flo_String propValue, flo_html_Dom *dom) {
    flo_html_index_id propKeyID =
        (flo_html_index_id)flo_containsStringHashSet(&dom->propKeys, propKey);
    if (propKeyID == 0) {
        return false;
    }

    flo_html_index_id propValueID =
        (flo_html_index_id)flo_containsStringHashSet(&dom->propValues,
                                                     propValue);
    if (propKeyID == 0) {
        return false;
    }

    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        flo_html_Property *property = &dom->props.buf[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID &&
            property->valueID == propValueID) {
            return true;
        }
    }
    return false;
}

flo_String_d_a flo_html_getTextContent(flo_html_node_id nodeID,
                                       flo_html_Dom *dom, flo_Arena *perm) {
    flo_String_d_a results = {0};

    flo_html_node_id currentNodeID = nodeID;
    while ((currentNodeID =
                flo_html_traverseNode(currentNodeID, nodeID, dom)) != 0) {
        flo_html_Node node = dom->nodes.buf[currentNodeID];

        if (node.nodeType == NODE_TYPE_TEXT) {
            *FLO_PUSH(&results, perm) = node.text;
        }
    }

    return results;
}

flo_String flo_html_getValue(flo_html_node_id nodeID, flo_String propKey,
                             flo_html_Dom *dom) {
    flo_html_index_id propKeyID =
        (flo_html_index_id)flo_containsStringHashSet(&dom->propKeys, propKey);
    if (propKeyID == 0) {
        return FLO_EMPTY_STRING;
    }

    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        flo_html_Property property = dom->props.buf[i];
        if (property.nodeID == nodeID && property.keyID == propKeyID) {
            return dom->propValueRegistry.buf[property.valueID];
        }
    }
    return FLO_EMPTY_STRING;
}
