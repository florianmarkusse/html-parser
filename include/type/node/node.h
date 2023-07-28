#ifndef TYPE_NODE_NODE_H
#define TYPE_NODE_NODE_H

#include <stddef.h>
#include <stdint.h>

#include "../element/elements.h"

typedef uint16_t node_id;

typedef struct {
    node_id nodeID;
    element_id tagID;
} __attribute__((aligned(4))) Node;

#endif