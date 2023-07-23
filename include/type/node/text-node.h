#ifndef TYPE_NODE_TEXT_NODE_H
#define TYPE_NODE_TEXT_NODE_H

#include "node.h"

typedef struct {
    node_id nodeID;
    element_id textID;
} __attribute__((aligned(4))) TextNode;

#endif
