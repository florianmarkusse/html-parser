#ifndef DOM_QUERYING_DOCUMENT_QUERYING_UTIL_H
#define DOM_QUERYING_DOCUMENT_QUERYING_UTIL_H

#include "dom/document.h"
#include "dom/querying/document-querying-status.h"
#include "type/element/element-status.h"
#include "type/element/elements-container.h"

QueryingStatus getTagID(const char *tag, element_id *tagID);
QueryingStatus getNodesWithTagID(element_id tagID, const Document *doc,
                                 node_id **results, size_t *len,
                                 size_t *currentCap);

QueryingStatus filterByTagID(element_id tagID, const Document *doc,
                             node_id *results, size_t *len);
QueryingStatus getDescendantsOf(node_id **results, size_t *len,
                                size_t *currentCap, Document *doc);
#endif
