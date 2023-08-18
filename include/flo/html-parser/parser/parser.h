#ifndef FLO_HTML_PARSER_PARSER_PARSER_H
#define FLO_HTML_PARSER_PARSER_PARSER_H

#include "flo/html-parser/dom/dom.h"

#define MAX_NODE_DEPTH 1U << 7U
#define MAX_PROPERTIES 1U << 7U

DomStatus parse(const char *htmlString, Dom *dom, DataContainer *dataContainer);

#endif
