#ifndef FLO_HTML_PARSER_PARSER_H
#define FLO_HTML_PARSER_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/node/document-node.h"
#include "flo/html-parser/user.h"

#define FLO_HTML_MAX_NODE_DEPTH 1U << 7U
#define FLO_HTML_MAX_PROPERTIES 1U << 7U

typedef struct {
    flo_html_node_id nodeID;
    flo_html_String tag;
} flo_html_TagAndNodeID;

typedef struct {
    flo_html_TagAndNodeID stack[FLO_HTML_MAX_NODE_DEPTH];
    ptrdiff_t len;
} flo_html_NodeDepth;

void flo_html_parseRoot(const flo_html_String html, flo_html_ParsedHTML parsed,
                        flo_html_Arena *perm);
void flo_html_parseExtra(const flo_html_String html, flo_html_ParsedHTML parsed,
                         flo_html_Arena *perm);

flo_html_node_id
flo_html_parseDocumentElement(const flo_html_DocumentNode *documentNode,
                              flo_html_ParsedHTML parsed, flo_html_Arena *perm);
flo_html_node_id flo_html_parseTextElement(flo_html_String text,
                                           flo_html_ParsedHTML parsed,
                                           flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
