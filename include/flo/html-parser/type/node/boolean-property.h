#ifndef FLO_HTML_PARSER_TYPE_NODE_BOOLEAN_PROPERTY_H
#define FLO_HTML_PARSER_TYPE_NODE_BOOLEAN_PROPERTY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/type/element/elements.h"
#include "node.h"

typedef struct {
    flo_html_node_id nodeID;
    flo_html_element_id propID;
} __attribute__((aligned(4))) flo_html_BooleanProperty;

#ifdef __cplusplus
}
#endif

#endif
