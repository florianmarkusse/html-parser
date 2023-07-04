#ifndef TOKENIZER_PARSE_H
#define TOKENIZER_PARSE_H

#include "dom/document.h"
void parseNodes(const char *htmlString, Document *doc);
void parseNodesNew(const char *htmlString, Document *doc);

#endif
