#ifndef FLO_HTML_PARSER_NODE_NEXT_NODE_H
#define FLO_HTML_PARSER_NODE_NEXT_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "node.h"

typedef struct {
    flo_html_node_id currentNodeID;
    flo_html_node_id nextNodeID;
} flo_html_NextNode;

#ifdef __cplusplus
}
#endif

#endif
