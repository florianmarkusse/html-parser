#ifndef FLO_HTML_PARSER_DOM_READING_READING_UTIL_H
#define FLO_HTML_PARSER_DOM_READING_READING_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/node/boolean-property.h"
#include "flo/html-parser/node/property.h"

flo_html_Property *flo_html_getProperty(flo_html_node_id nodeID,
                                        flo_html_index_id propKeyID,
                                        const flo_html_Dom *dom);
flo_html_BooleanProperty *
flo_html_getBooleanProperty(flo_html_node_id nodeID,
                            flo_html_index_id boolPropID,
                            const flo_html_Dom *dom);

#ifdef __cplusplus
}
#endif

#endif
