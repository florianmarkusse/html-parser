#ifndef FLO_HTML_PARSER_PARSE_PARSE_H
#define FLO_HTML_PARSER_PARSE_PARSE_H

#include "flo/html-parser/dom/document.h"

#define MAX_NODE_DEPTH 1U << 7U
#define MAX_PROPERTIES 1U << 7U

DocumentStatus parse(const char *htmlString, Document *doc,
                     DataContainer *dataContainer);

#endif
