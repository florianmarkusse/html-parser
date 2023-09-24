#ifndef FLO_HTML_PARSER_TYPE_NODE_NODE_H
#define FLO_HTML_PARSER_TYPE_NODE_NODE_H

#include <stddef.h>
#include <stdint.h>

#include "flo/html-parser/type/data/definitions.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint16_t flo_html_node_id;

typedef enum {
    NODE_TYPE_DOCUMENT,
    NODE_TYPE_TEXT,
    NODE_TYPE_REMOVED,
    NODE_TYPE_ERROR,
    NODE_TYPE_NUM
} flo_html_NodeType;

static const char *const nodeTypeStrings[NODE_TYPE_NUM] = {"Document", "Text",
                                                           "Removed", "Error"};

__attribute__((unused)) static const char *
flo_html_nodeTypeToString(flo_html_NodeType type) {
    if (type >= 0 && type < NODE_TYPE_NUM) {
        return nodeTypeStrings[type];
    }
    return "Unknown node type!";
}

typedef struct {
    flo_html_node_id nodeID;
    flo_html_NodeType nodeType;
    union {
        flo_html_indexID tagID;
        const char *text;
    };
} flo_html_Node;

#ifdef __cplusplus
}
#endif

#endif
