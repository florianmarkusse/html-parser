#ifndef TYPE_NODE_TYPE_H
#define TYPE_NODE_TYPE_H

#include <stddef.h>
#include <stdint.h>

typedef uint16_t NodeType;

#define NODE_TYPE_MSB (1U << ((sizeof(NodeType) * 8) - 1))
#define NODE_TYPE_NUM_BITS (sizeof(NodeType) * 8)

// Paired tags
#define HTML 0x00
#define TITLE 0x01
#define BODY 0x02
#define H1 0x03
#define HEAD 0x04
#define DIV 0x05
#define P 0x06

// Self-closing tags
#define DOCTYPE NODE_TYPE_MSB
#define META (NODE_TYPE_MSB + 0x01)
#define INPUT (NODE_TYPE_MSB + 0x02)

// Utility tags
#define UNKNOWN ((NodeType)-1)

NodeType mapStringToType(const char *str, size_t strLen);
const char *mapTypeToString(NodeType type);
unsigned char isSelfClosing(NodeType type);

#endif
