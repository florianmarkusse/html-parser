#ifndef TYPE_PARENT_FIRST_CHILD_H
#define TYPE_PARENT_FIRST_CHILD_H

typedef struct {
    unsigned int parentID;
    unsigned int childID;
} __attribute__((aligned(8))) ParentFirstChild;

#endif
