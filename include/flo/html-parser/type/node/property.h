#ifndef FLO_HTML_PARSER_TYPE_NODE_PROPERTY_H
#define FLO_HTML_PARSER_TYPE_NODE_PROPERTY_H

#include "node.h"

typedef struct {
    node_id nodeID;
    element_id keyID;
    element_id valueID;
} __attribute__((aligned(8))) Property;

#endif