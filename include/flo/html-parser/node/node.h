#ifndef FLO_HTML_PARSER_NODE_NODE_H
#define FLO_HTML_PARSER_NODE_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "flo/html-parser/definitions.h"
#include "text/string.h"

typedef enum {
    NODE_TYPE_ROOT,
    NODE_TYPE_DOCUMENT,
    NODE_TYPE_TEXT,
    NODE_TYPE_REMOVED,
    NODE_TYPE_ERROR,
    NODE_TYPE_NUM
} flo_html_NodeType;

static flo_String nodeTypeStrings[NODE_TYPE_NUM] = {
    FLO_STRING("Root"), FLO_STRING("Document"), FLO_STRING("Text"),
    FLO_STRING("Removed"), FLO_STRING("Error")};

__attribute__((unused)) static flo_String
flo_html_nodeTypeToString(flo_html_NodeType type) {
    if (type >= 0 && type < NODE_TYPE_NUM) {
        return nodeTypeStrings[type];
    }
    return FLO_STRING("Unknown node type!");
}

typedef struct {
    flo_html_node_id nodeID;
    flo_html_NodeType nodeType;
    union {
        flo_html_index_id tagID;
        flo_String text;
    };
} flo_html_Node;

#ifdef __cplusplus
}
#endif

#endif
