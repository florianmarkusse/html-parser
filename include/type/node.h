#ifndef TYPE_NODE_H
#define TYPE_NODE_H

#include <stddef.h>
#include <stdint.h>

#include "tag.h"

typedef uint16_t node_id;

typedef struct {
    node_id nodeID;
    tag_id tagID;
} __attribute__((aligned(4))) Node;

#endif
