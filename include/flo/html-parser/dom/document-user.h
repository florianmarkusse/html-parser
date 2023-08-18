#ifndef FLO_HTML_PARSER_DOM_DOCUMENT_USER_H
#define FLO_HTML_PARSER_DOM_DOCUMENT_USER_H

#include "document.h"

DocumentStatus createFromFile(const char *fileLocation, Document *doc,
                              DataContainer *dataContainer);

#endif
