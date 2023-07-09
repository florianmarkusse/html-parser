#ifndef TYPE_NODE_ATTRIBUTE_H
#define TYPE_NODE_ATTRIBUTE_H

#include "node.h"

typedef struct {
    node_id nodeID;
    element_id attributeID;
} __attribute__((aligned(4))) NodeAttribute;

#endif
