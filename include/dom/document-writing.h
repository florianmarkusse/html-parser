#ifndef DOM_DOCUMENT_WRITING_H
#define DOM_DOCUMENT_WRITING_H

#include "document.h"

void printDocumentStatus(const Document *doc);
void printXML(const Document *doc);
FileStatus writeXMLToFile(const Document *doc, const char *filePath);

#endif
