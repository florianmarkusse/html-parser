#ifndef FLO_HTML_PARSER_TYPE_NODE_NODE_H
#define FLO_HTML_PARSER_TYPE_NODE_NODE_H

#include <stddef.h>
#include <stdint.h>

#include "flo/html-parser/type/data/definitions.h"
#include "flo/html-parser/type/element/elements.h"

typedef uint16_t node_id;

typedef enum {
    NODE_TYPE_DOCUMENT,
    NODE_TYPE_TEXT,
    NODE_TYPE_ERROR,
    NODE_TYPE_NUM
} NodeType;

static const char *const NodeTypeStrings[NODE_TYPE_NUM] = {"Document", "Text",
                                                           "Error"};

__attribute__((unused)) static const char *nodeTypeToString(NodeType type) {
    if (type >= 0 && type < NODE_TYPE_NUM) {
        return NodeTypeStrings[type];
    }
    return "Unknown node type!";
}

typedef struct {
    node_id nodeID;
    NodeType nodeType;
    indexID indexID;
} __attribute__((aligned(8))) Node;

#endif
