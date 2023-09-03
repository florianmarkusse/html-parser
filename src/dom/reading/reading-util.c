#include "flo/html-parser/dom/reading/reading-util.h"
#include "flo/html-parser/type/node/boolean-property.h"

Property *getProperty(const node_id nodeID, const element_id propKeyID,
                      const Dom *dom) {
    for (size_t i = 0; i < dom->propsLen; i++) {
        Property *prop = &dom->props[i];

        if (prop->nodeID == nodeID && prop->keyID == propKeyID) {
            return prop;
        }
    }

    return NULL;
}

BooleanProperty *getBooleanProperty(node_id nodeID, element_id boolPropID,
                                    const Dom *dom) {
    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        BooleanProperty *prop = &dom->boolProps[i];

        if (prop->nodeID == nodeID && prop->propID == boolPropID) {
            return prop;
        }
    }

    return NULL;
}
