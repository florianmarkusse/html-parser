#ifndef FLO_HTML_PARSER_PARSER_PARSER_H
#define FLO_HTML_PARSER_PARSER_PARSER_H

#include "flo/html-parser/dom/dom.h"

#define MAX_NODE_DEPTH 1U << 7U
#define MAX_PROPERTIES 1U << 7U

typedef struct {
    node_id stack[MAX_NODE_DEPTH];
    size_t len;
} __attribute__((aligned(128))) NodeDepth;

DomStatus parseFromRoot(const char *htmlString, Dom *dom,
                        DataContainer *dataContainer);
DomStatus parse(const char *htmlString, Dom *dom, DataContainer *dataContainer,
                NodeDepth *nodeStack, node_id lastParsedNodeID);

#endif
