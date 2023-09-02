#include "flo/html-parser/dom/reading/reading-util.h"

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
