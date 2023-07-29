#ifndef DOM_QUERYING_DOCUMENT_QUERYING_H
#define DOM_QUERYING_DOCUMENT_QUERYING_H

#include "document-querying-status.h"
#include "dom/document.h"

QueryingStatus querySelectorAll(Document *doc, const char *cssQuery);

#endif
