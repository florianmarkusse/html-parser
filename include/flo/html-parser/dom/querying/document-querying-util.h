#ifndef FLO_HTML_PARSER_DOM_QUERYING_DOCUMENT_QUERYING_UTIL_H
#define FLO_HTML_PARSER_DOM_QUERYING_DOCUMENT_QUERYING_UTIL_H

#include <stdbool.h>

#include "flo/html-parser/dom/document.h"
#include "flo/html-parser/dom/querying/document-querying-status.h"
#include "flo/html-parser/dom/querying/document-querying.h"
#include "flo/html-parser/type/element/element-status.h"
#include "flo/html-parser/type/element/elements-container.h"

bool filterNode(node_id nodeID, const FilterType *filters, size_t filterslen,
                const Document *doc);
QueryingStatus getTagID(const char *tag, element_id *tagID,
                        const DataContainer *dataContainer);
QueryingStatus getBoolPropID(const char *tag, element_id *propID,
                             const DataContainer *dataContainer);
QueryingStatus getKeyPropID(const char *tag, element_id *keyID,
                            const DataContainer *dataContainer);
QueryingStatus getValuePropID(const char *tag, element_id *valueID,
                              const DataContainer *dataContainer);
QueryingStatus getNodesWithTagID(element_id tagID, const Document *doc,
                                 node_id **results, size_t *len,
                                 size_t *currentCap);

QueryingStatus filterByTagID(element_id tagID, const Document *doc,
                             node_id *results, size_t *len);
QueryingStatus
getNodesWithoutCombinator(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                          size_t filtersLen, const Document *doc,
                          node_id **results, size_t *len, size_t *currentCap);

QueryingStatus
getFilteredAdjacents(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                     size_t filtersLen, const Document *doc,
                     size_t numberOfSiblings, node_id **results, size_t *len,
                     size_t *currentCap);
QueryingStatus
getFilteredDescendants(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                       size_t filtersLen, const Document *doc, size_t depth,
                       node_id **results, size_t *len, size_t *currentCap);
#endif
