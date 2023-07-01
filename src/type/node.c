#include <string.h>

#include "type/node.h"

typedef struct {
    const char *name;
    NodeType type;
} __attribute__((aligned(16))) NodeTypeMapping;

static const NodeTypeMapping nodeTypeMappings[] = {
    {"DOCTYPE", DOCTYPE}, {"html", HTML}, {"meta", META},
    {"title", TITLE},     {"body", BODY}, {"h1", H1},
};

static const size_t NUM_MAPPINGS =
    sizeof(nodeTypeMappings) / sizeof(nodeTypeMappings[0]);

NodeType mapStringToType(const char *str, size_t strLen) {
    for (size_t i = 0; i < NUM_MAPPINGS; i++) {
        if (strncmp(str, nodeTypeMappings[i].name, strLen) == 0) {
            return nodeTypeMappings[i].type;
        }
    }

    return UNKNOWN;
}

const char *mapTypeToString(NodeType type) {
    for (size_t i = 0; i < NUM_MAPPINGS; i++) {
        if (type == nodeTypeMappings[i].type) {
            return nodeTypeMappings[i].name;
        }
    }

    return "UNKNOWN";
}
