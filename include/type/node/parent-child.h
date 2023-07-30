#ifndef TYPE_NODE_PARENT_CHILD_H
#define TYPE_NODE_PARENT_CHILD_H

#include "node.h"

typedef struct {
    node_id parentID;
    node_id childID;
} __attribute__((aligned(4))) ParentChild;

#endif
