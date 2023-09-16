#ifndef FLO_HTML_PARSER_PARSER_PARSER_H
#define FLO_HTML_PARSER_PARSER_PARSER_H

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/document-node.h"

#define FLO_HTML_MAX_NODE_DEPTH 1U << 7U
#define FLO_HTML_MAX_PROPERTIES 1U << 7U

typedef struct {
    flo_html_node_id stack[FLO_HTML_MAX_NODE_DEPTH];
    size_t len;
} __attribute__((aligned(128))) flo_html_NodeDepth;

flo_html_DomStatus flo_html_parse(const char *htmlString, flo_html_Dom *dom,
                                  flo_html_TextStore *textStore);
flo_html_DomStatus
flo_html_parseDocumentElement(const flo_html_DocumentNode *documentNode,
                              flo_html_Dom *dom, flo_html_TextStore *textStore,
                              flo_html_node_id *newNodeID);
flo_html_DomStatus flo_html_parseTextElement(const char *text,
                                             flo_html_Dom *dom,
                                             flo_html_TextStore *textStore,
                                             flo_html_node_id *newNodeID);

#endif
