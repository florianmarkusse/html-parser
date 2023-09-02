#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/node/node-reading.h"
#include "flo/html-parser/dom/query/dom-query-status.h"
#include "flo/html-parser/dom/query/dom-query-util.h"
#include "flo/html-parser/utils/memory/memory.h"

Node getNode(const node_id nodeID, const Dom *dom) {
    return dom->nodes[nodeID];
}

NodeType getNodeType(const node_id nodeID, const Dom *dom) {
    return dom->nodes[nodeID].nodeType;
}

bool hasBoolProp(const node_id nodeID, const char *boolProp, const Dom *dom,
                 const DataContainer *dataContainer) {
    element_id boolPropID = 0;
    if (getBoolPropID(boolProp, &boolPropID, dataContainer) != QUERY_SUCCESS) {
        return false;
    }

    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        BooleanProperty *booleanProperty = &dom->boolProps[i];
        if (booleanProperty->nodeID == nodeID &&
            booleanProperty->propID == boolPropID) {
            return true;
        }
    }
    return false;
}

bool hasPropKey(const node_id nodeID, const char *propKey, const Dom *dom,
                const DataContainer *dataContainer) {
    element_id propKeyID = 0;
    if (getPropKeyID(propKey, &propKeyID, dataContainer) != QUERY_SUCCESS) {
        return false;
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID) {
            return true;
        }
    }
    return false;
}

bool hasPropValue(const node_id nodeID, const char *propValue, const Dom *dom,
                  const DataContainer *dataContainer) {
    element_id propValueID = 0;
    if (getPropValueID(propValue, &propValueID, dataContainer) !=
        QUERY_SUCCESS) {
        return false;
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->valueID == propValueID) {
            return true;
        }
    }
    return false;
}

bool hasProperty(node_id nodeID, const char *propKey, const char *propValue,
                 const Dom *dom, const DataContainer *dataContainer) {
    element_id propKeyID = 0;
    if (getPropKeyID(propKey, &propKeyID, dataContainer) != QUERY_SUCCESS) {
        return false;
    }

    element_id propValueID = 0;
    if (getPropValueID(propValue, &propValueID, dataContainer) !=
        QUERY_SUCCESS) {
        return false;
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID &&
            property->valueID == propValueID) {
            return true;
        }
    }
    return false;
}

QueryStatus getTextContent(const node_id nodeID, const Dom *dom,
                           const char ***results, size_t *reusultsLen) {
    node_id currentNodeID = nodeID;
    size_t currentCap = 0;
    while ((currentNodeID = traverseNode(currentNodeID, nodeID, dom)) != 0) {
        Node node = dom->nodes[currentNodeID];

        if (node.nodeType == NODE_TYPE_TEXT) {
            if ((*results = resizeArray(*results, *reusultsLen, &currentCap,
                                        sizeof(const char *), 64)) == NULL) {
                PRINT_ERROR("Failed to allocate memory for results array!\n");
                return QUERY_MEMORY_ERROR;
            }

            (*results)[(*reusultsLen)] = node.text;
            (*reusultsLen)++;
        }
    }

    return QUERY_SUCCESS;
}

const char *getValue(const node_id nodeID, const char *propKey, const Dom *dom,
                     const DataContainer *dataContainer) {
    element_id propKeyID = 0;
    if (getPropKeyID(propKey, &propKeyID, dataContainer) != QUERY_SUCCESS) {
        return NULL;
    }

    for (size_t i = 0; i < dom->propsLen; i++) {
        Property *property = &dom->props[i];
        if (property->nodeID == nodeID && property->keyID == propKeyID) {
            return getPropValue(property->valueID, dom, dataContainer);
        }
    }
    return NULL;
}
