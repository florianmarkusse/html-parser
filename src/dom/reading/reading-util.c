#include "flo/html-parser/dom/reading/reading-util.h"
#include "flo/html-parser/node/boolean-property.h"

flo_html_Property *flo_html_getProperty(const flo_html_node_id nodeID,
                                        const flo_html_element_id propKeyID,
                                        const flo_html_Dom *dom) {
    for (ptrdiff_t i = 0; i < dom->propsLen; i++) {
        flo_html_Property *prop = &dom->props[i];

        if (prop->nodeID == nodeID && prop->keyID == propKeyID) {
            return prop;
        }
    }

    return NULL;
}

flo_html_BooleanProperty *
flo_html_getBooleanProperty(flo_html_node_id nodeID,
                            flo_html_element_id boolPropID,
                            const flo_html_Dom *dom) {
    for (ptrdiff_t i = 0; i < dom->boolPropsLen; i++) {
        flo_html_BooleanProperty *prop = &dom->boolProps[i];

        if (prop->nodeID == nodeID && prop->propID == boolPropID) {
            return prop;
        }
    }

    return NULL;
}
