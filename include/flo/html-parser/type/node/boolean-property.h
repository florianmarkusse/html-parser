#ifndef FLO_HTML_PARSER_TYPE_NODE_BOOLEAN_PROPERTY_H
#define FLO_HTML_PARSER_TYPE_NODE_BOOLEAN_PROPERTY_H

#include "node.h"

typedef struct {
    node_id nodeID;
    element_id propID;
} __attribute__((aligned(4))) BooleanProperty;

#endif
