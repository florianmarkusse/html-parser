#ifndef DOM_DOCUMENT_WRITING_H
#define DOM_DOCUMENT_WRITING_H

#include "document.h"

void printDocumentStatus(const Document *doc,
                         const DataContainer *dataContainer);
void printHTML(const Document *doc, const DataContainer *dataContainer);
FileStatus writeHTMLToFile(const Document *doc,
                           const DataContainer *dataContainer,
                           const char *filePath);

#endif
