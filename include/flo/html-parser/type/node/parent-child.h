#ifndef FLO_HTML_PARSER_TYPE_NODE_PARENT_CHILD_H
#define FLO_HTML_PARSER_TYPE_NODE_PARENT_CHILD_H

#include "node.h"

typedef struct {
    flo_html_node_id parentID;
    flo_html_node_id childID;
} __attribute__((aligned(4))) flo_html_ParentChild;

#endif
