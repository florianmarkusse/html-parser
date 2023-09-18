#ifndef FLO_HTML_PARSER_TYPE_NODE_PROPERTY_H
#define FLO_HTML_PARSER_TYPE_NODE_PROPERTY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "node.h"

typedef struct {
    flo_html_node_id nodeID;
    flo_html_indexID keyID;
    flo_html_indexID valueID;
} __attribute__((aligned(8))) flo_html_Property;

#ifdef __cplusplus
}
#endif

#endif
