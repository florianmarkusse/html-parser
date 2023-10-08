#ifndef FLO_HTML_PARSER_NODE_TEXT_NODE_H
#define FLO_HTML_PARSER_NODE_TEXT_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "node.h"

typedef struct {
    flo_html_node_id nodeID;
    flo_html_element_id textID;
} flo_html_TextNode;

#ifdef __cplusplus
}
#endif

#endif
