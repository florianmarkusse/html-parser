#ifndef TYPE_NODE_ATTRIBUTE_VALUE_H
#define TYPE_NODE_ATTRIBUTE_VALUE_H

#include "node.h"

typedef struct {
    node_id nodeID;
    element_id attributeID;
    element_id valueID;
} __attribute__((aligned(8))) NodeAttributeValue;

#endif
