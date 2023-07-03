#ifndef TYPE_NODE_H
#define TYPE_NODE_H

#include "node-type.h"

typedef struct {
    unsigned int ID;
    NodeType type;
} __attribute__((aligned(8))) Node;

#endif
