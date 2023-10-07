#ifndef FLO_HTML_PARSER_TYPE_NODE_NODE_H
#define FLO_HTML_PARSER_TYPE_NODE_NODE_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/util/text/string.h"

typedef enum {
    NODE_TYPE_ROOT,
    NODE_TYPE_DOCUMENT,
    NODE_TYPE_TEXT,
    NODE_TYPE_REMOVED,
    NODE_TYPE_ERROR,
    NODE_TYPE_NUM
} flo_html_NodeType;

static const flo_html_String nodeTypeStrings[NODE_TYPE_NUM] = {
    FLO_HTML_S("Root"), FLO_HTML_S("Document"), FLO_HTML_S("Text"),
    FLO_HTML_S("Removed"), FLO_HTML_S("Error")};

__attribute__((unused)) static const flo_html_String
flo_html_nodeTypeToString(flo_html_NodeType type) {
    if (type >= 0 && type < NODE_TYPE_NUM) {
        return nodeTypeStrings[type];
    }
    return FLO_HTML_S("Unknown node type!");
}

typedef struct {
    flo_html_node_id nodeID;
    flo_html_NodeType nodeType;
    union {
        flo_html_indexID tagID;
        flo_html_String text;
    };
} flo_html_Node;

#ifdef __cplusplus
}
#endif

#endif
