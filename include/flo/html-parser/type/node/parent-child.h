#ifndef FLO_HTML_PARSER_TYPE_NODE_PARENT_CHILD_H
#define FLO_HTML_PARSER_TYPE_NODE_PARENT_CHILD_H

#ifdef __cplusplus
extern "C" {
#endif

#include "node.h"

typedef struct {
    flo_html_node_id parentID;
    flo_html_node_id childID;
} flo_html_ParentChild;

#ifdef __cplusplus
}
#endif

#endif
