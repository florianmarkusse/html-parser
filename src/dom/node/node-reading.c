

#include "flo/html-parser/dom/node/node-reading.h"
#include "flo/html-parser/dom/dom.h"
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
