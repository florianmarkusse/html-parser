#ifndef DOM_QUERYING_DOCUMENT_QUERYING_H
#define DOM_QUERYING_DOCUMENT_QUERYING_H

#include "document-querying-status.h"
#include "dom/document.h"

#define INITIAL_QUERY_CAP (1U << 6U)

QueryingStatus querySelectorAll(Document *doc, const char *cssQuery);

#endif
