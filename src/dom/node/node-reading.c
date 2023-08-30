

#include "flo/html-parser/dom/node/node-reading.h"
#include "flo/html-parser/dom/dom-utils.h"
#include "flo/html-parser/dom/query/dom-query-util.h"

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
