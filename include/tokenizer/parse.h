#ifndef TOKENIZER_PARSE_H
#define TOKENIZER_PARSE_H

#include "dom/document.h"

#define MAX_NODE_DEPTH 1U << 7U
#define MAX_PROPERTIES 1U << 7U

DocumentStatus parse(const char *xmlString, Document *doc);

#endif
