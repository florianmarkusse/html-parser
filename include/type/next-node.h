#ifndef TYPE_NEXT_NODE_H
#define TYPE_NEXT_NODE_H

typedef struct {
    unsigned int currentNodeID;
    unsigned int nextNodeID;
} __attribute__((aligned(8))) NextNode;

#endif
