#ifndef FLO_HTML_PARSER_TYPE_NODE_TEXT_NODE_H
#define FLO_HTML_PARSER_TYPE_NODE_TEXT_NODE_H

#include "flo/html-parser/type/element/elements-container.h"
#include "node.h"

typedef struct {
    flo_html_node_id nodeID;
    flo_html_element_id textID;
} __attribute__((aligned(4))) flo_html_TextNode;

#endif
