#ifndef TYPE_NODE_NEXT_NODE_H
#define TYPE_NODE_NEXT_NODE_H

#include "node.h"

typedef struct {
    node_id currentNodeID;
    node_id nextNodeID;
} __attribute__((aligned(4))) NextNode;

#endif
