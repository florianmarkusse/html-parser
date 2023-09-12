#ifndef FLO_HTML_PARSER_PARSER_PARSER_H
#define FLO_HTML_PARSER_PARSER_PARSER_H

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/document-node.h"

#define MAX_NODE_DEPTH 1U << 7U
#define MAX_PROPERTIES 1U << 7U

typedef struct {
    node_id stack[MAX_NODE_DEPTH];
    size_t len;
} __attribute__((aligned(128))) NodeDepth;

DomStatus parse(const char *htmlString, Dom *dom, TextStore *textStore);
DomStatus parseDocumentElement(const DocumentNode *documentNode, Dom *dom,
                               TextStore *textStore,
                               node_id *newNodeID);
DomStatus parseTextElement(const char *text, Dom *dom,
                           TextStore *textStore, node_id *newNodeID);

#endif
