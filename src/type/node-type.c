#include <string.h>

#include "type/node-type.h"

typedef struct {
    const char *name;
    NodeType type;
} __attribute__((aligned(16))) NodeTypeMapping;

static const NodeTypeMapping nodeTypeMappings[] = {
    // paired tags
    {"html", HTML},
    {"title", TITLE},
    {"body", BODY},
    {"h1", H1},
    {"head", HEAD},
    {"div", DIV},
    {"p", P},
    // self-closing
    {"!DOCTYPE", DOCTYPE},
    {"meta", META},
    {"input", INPUT},
};

static const size_t NUM_MAPPINGS =
    sizeof(nodeTypeMappings) / sizeof(nodeTypeMappings[0]);

NodeType mapStringToType(const char *str, const size_t strLen) {
    for (size_t i = 0; i < NUM_MAPPINGS; i++) {
        if (strncmp(str, nodeTypeMappings[i].name, strLen) == 0) {
            return nodeTypeMappings[i].type;
        }
    }

    return UNKNOWN;
}

const char *mapTypeToString(const NodeType type) {
    for (size_t i = 0; i < NUM_MAPPINGS; i++) {
        if (type == nodeTypeMappings[i].type) {
            return nodeTypeMappings[i].name;
        }
    }

    return "UNKNOWN";
}

unsigned char isSelfClosing(const NodeType type) {
    return type >> (NODE_TYPE_NUM_BITS - 1) != 0;
}
