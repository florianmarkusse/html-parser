#include "flo/html-parser/dom/reading/reading-util.h"
#include "flo/html-parser/node/boolean-property.h"

flo_html_Property *flo_html_getProperty(flo_html_node_id nodeID,
                                        flo_html_index_id propKeyID,
                                        flo_html_Dom *dom) {
    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        flo_html_Property *prop = &dom->props.buf[i];

        if (prop->nodeID == nodeID && prop->keyID == propKeyID) {
            return prop;
        }
    }

    return NULL;
}

flo_html_BooleanProperty *
flo_html_getBooleanProperty(flo_html_node_id nodeID,
                            flo_html_index_id boolPropID, flo_html_Dom *dom) {
    for (ptrdiff_t i = 0; i < dom->boolProps.len; i++) {
        flo_html_BooleanProperty *prop = &dom->boolProps.buf[i];

        if (prop->nodeID == nodeID && prop->propID == boolPropID) {
            return prop;
        }
    }

    return NULL;
}
