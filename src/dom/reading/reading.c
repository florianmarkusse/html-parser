#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/reading/reading.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/util/memory.h"

flo_html_NodeType flo_html_getflo_html_NodeType(const flo_html_node_id nodeID,
                                                const flo_html_Dom *dom) {
    return dom->nodes[nodeID].nodeType;
}

bool flo_html_hasBoolProp(const flo_html_node_id nodeID,
                          const flo_html_String boolProp,
                          const flo_html_Dom *dom,
                          const flo_html_TextStore *textStore) {
    flo_html_index_id boolPropID =
        flo_html_getEntryID(boolProp, &textStore->boolProps.set);
    if (boolPropID == 0) {
        return false;
    }

    for (ptrdiff_t i = 0; i < dom->boolPropsLen; i++) {
        flo_html_BooleanProperty *booleanProperty = &dom->boolProps[i];
        if (booleanProperty->nodeID == nodeID &&
            booleanProperty->propID == boolPropID) {
            return true;
        }
    }
    return false;
}

bool flo_html_hasPropKey(const flo_html_node_id nodeID,
                         const flo_html_String propKey, const flo_html_Dom *dom,
                         const flo_html_TextStore *textStore) {
    flo_html_index_id propKeyID =
        flo_html_getEntryID(propKey, &textStore->propKeys.set);
    if (propKeyID == 0) {
        return false;
    }

    for (ptrdiff_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID) {
            return true;
        }
    }
    return false;
}

bool flo_html_hasPropValue(const flo_html_node_id nodeID,
                           const flo_html_String propValue,
                           const flo_html_Dom *dom,
                           const flo_html_TextStore *textStore) {
    flo_html_index_id propValueID =
        flo_html_getEntryID(propValue, &textStore->propValues.set);
    if (propValueID == 0) {
        return false;
    }

    for (ptrdiff_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->valueID == propValueID) {
            return true;
        }
    }
    return false;
}

bool flo_html_hasProperty(flo_html_node_id nodeID,
                          const flo_html_String propKey,
                          const flo_html_String propValue,
                          const flo_html_Dom *dom,
                          const flo_html_TextStore *textStore) {
    flo_html_index_id propKeyID =
        flo_html_getEntryID(propKey, &textStore->propKeys.set);
    if (propKeyID == 0) {
        return false;
    }

    flo_html_index_id propValueID =
        flo_html_getEntryID(propValue, &textStore->propValues.set);
    if (propKeyID == 0) {
        return false;
    }

    for (ptrdiff_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID &&
            property->valueID == propValueID) {
            return true;
        }
    }
    return false;
}

// TODO: Only use the final result on the perm aerna.
flo_html_QueryStatus flo_html_getTextContent(const flo_html_node_id nodeID,
                                             const flo_html_Dom *dom,
                                             flo_html_String_da *results,
                                             flo_html_Arena *perm) {
    flo_html_node_id currentNodeID = nodeID;
    while ((currentNodeID =
                flo_html_traverseNode(currentNodeID, nodeID, dom)) != 0) {
        flo_html_Node node = dom->nodes[currentNodeID];

        if (node.nodeType == NODE_TYPE_TEXT) {
            *FLO_HTML_PUSH(results, perm) = node.text;
        }
    }

    return QUERY_SUCCESS;
}

const flo_html_String flo_html_getValue(const flo_html_node_id nodeID,
                                        const flo_html_String propKey,
                                        flo_html_ParsedHTML *parsed) {
    flo_html_index_id propKeyID =
        flo_html_containsStringHashSet(&parsed->textStore.propKeys.set, propKey)
            .entryIndex;
    if (propKeyID == 0) {
        return FLO_HTML_EMPTY_STRING;
    }

    for (ptrdiff_t i = 0; i < parsed->dom.propsLen; i++) {
        flo_html_Property *property = &parsed->dom.props[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID) {
            return flo_html_getPropValue(property->valueID, parsed);
        }
    }
    return FLO_HTML_EMPTY_STRING;
}
