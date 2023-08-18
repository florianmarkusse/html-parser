#ifndef FLO_HTML_PARSER_DOM_DOCUMENT_WRITING_H
#define FLO_HTML_PARSER_DOM_DOCUMENT_WRITING_H

#include "document.h"

void printDocumentStatus(const Document *doc,
                         const DataContainer *dataContainer);
void printHTML(const Document *doc, const DataContainer *dataContainer);
FileStatus writeHTMLToFile(const Document *doc,
                           const DataContainer *dataContainer,
                           const char *filePath);

#endif
