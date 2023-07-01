#ifndef TYPE_NODE_H
#define TYPE_NODE_H

#include <stddef.h>

typedef enum {
    DOCTYPE,
    HTML,
    META,
    TITLE,
    BODY,
    H1,
    UNKNOWN,
    NUM_NODE_TYPES,
} NodeType;

NodeType mapStringToType(const char *str, size_t strLen);
const char *mapTypeToString(NodeType type);

#endif
