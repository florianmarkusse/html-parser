#ifndef FLO_HTML_PARSER_NODE_PROPERTY_H
#define FLO_HTML_PARSER_NODE_PROPERTY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "node.h"

typedef struct {
    flo_html_node_id nodeID;
    flo_html_index_id keyID;
    flo_html_index_id valueID;
} flo_html_Property;

#ifdef __cplusplus
}
#endif

#endif
