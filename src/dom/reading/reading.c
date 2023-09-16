#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/reading/reading.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/utils/memory/memory.h"

flo_html_NodeType flo_html_getflo_html_NodeType(const flo_html_node_id nodeID,
                                                const flo_html_Dom *dom) {
    return dom->nodes[nodeID].nodeType;
}

bool flo_html_hasBoolProp(const flo_html_node_id nodeID, const char *boolProp,
                          const flo_html_Dom *dom,
                          const flo_html_TextStore *textStore) {
    flo_html_element_id boolPropID =
        flo_html_getBoolPropID(boolProp, textStore);
    if (boolPropID == 0) {
        return false;
    }

    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        flo_html_BooleanProperty *booleanProperty = &dom->boolProps[i];
        if (booleanProperty->nodeID == nodeID &&
            booleanProperty->propID == boolPropID) {
            return true;
        }
    }
    return false;
}

bool flo_html_hasPropKey(const flo_html_node_id nodeID, const char *propKey,
                         const flo_html_Dom *dom,
                         const flo_html_TextStore *textStore) {
    flo_html_element_id propKeyID = flo_html_getPropKeyID(propKey, textStore);
    if (propKeyID == 0) {
        return false;
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID) {
            return true;
        }
    }
    return false;
}

bool flo_html_hasPropValue(const flo_html_node_id nodeID, const char *propValue,
                           const flo_html_Dom *dom,
                           const flo_html_TextStore *textStore) {
    flo_html_element_id propValueID =
        flo_html_getPropValueID(propValue, textStore);
    if (propValueID == 0) {
        return false;
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->valueID == propValueID) {
            return true;
        }
    }
    return false;
}

bool flo_html_hasProperty(flo_html_node_id nodeID, const char *propKey,
                          const char *propValue, const flo_html_Dom *dom,
                          const flo_html_TextStore *textStore) {
    flo_html_element_id propKeyID = flo_html_getPropKeyID(propKey, textStore);
    if (propKeyID == 0) {
        return false;
    }

    flo_html_element_id propValueID =
        flo_html_getPropValueID(propValue, textStore);
    if (propKeyID == 0) {
        return false;
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID &&
            property->valueID == propValueID) {
            return true;
        }
    }
    return false;
}

flo_html_QueryStatus flo_html_getTextContent(const flo_html_node_id nodeID,
                                             const flo_html_Dom *dom,
                                             const char ***results,
                                             size_t *reusultsLen) {
    flo_html_node_id currentNodeID = nodeID;
    size_t currentCap = 0;
    while ((currentNodeID =
                flo_html_traverseNode(currentNodeID, nodeID, dom)) != 0) {
        flo_html_Node node = dom->nodes[currentNodeID];

        if (node.nodeType == NODE_TYPE_TEXT) {
            if ((*results =
                     flo_html_resizeArray(*results, *reusultsLen, &currentCap,
                                          sizeof(const char *), 64)) == NULL) {
                FLO_HTML_PRINT_ERROR(
                    "Failed to allocate memory for results array!\n");
                return QUERY_MEMORY_ERROR;
            }

            (*results)[(*reusultsLen)] = node.text;
            (*reusultsLen)++;
        }
    }

    return QUERY_SUCCESS;
}

const char *flo_html_getValue(const flo_html_node_id nodeID,
                              const char *propKey, const flo_html_Dom *dom,
                              const flo_html_TextStore *textStore) {
    flo_html_element_id propKeyID = flo_html_getPropKeyID(propKey, textStore);
    if (propKeyID == 0) {
        return NULL;
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID) {
            return flo_html_getPropValue(property->valueID, dom, textStore);
        }
    }
    return NULL;
}
