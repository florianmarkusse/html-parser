#ifndef FLO_HTML_PARSER_DOM_QUERY_QUERY_UTIL_H
#define FLO_HTML_PARSER_DOM_QUERY_QUERY_UTIL_H

#include <stdbool.h>

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/hash/uint16-t-hash.h"
#include "flo/html-parser/type/data/definitions.h"
#include "flo/html-parser/type/element/element-status.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "query-status.h"
#include "query.h"

bool filterNode(node_id nodeID, const FilterType *filters, size_t filterslen,
                const Dom *dom);
indexID getTagID(const char *tag, const DataContainer *dataContainer);
indexID getBoolPropID(const char *boolProp, const DataContainer *dataContainer);
indexID getPropKeyID(const char *keyProp, const DataContainer *dataContainer);
indexID getPropValueID(const char *valueProp,
                       const DataContainer *dataContainer);

QueryStatus filterByTagID(element_id tagID, const Dom *dom, node_id *results,
                          size_t *len);
QueryStatus
getNodesWithoutCombinator(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                          size_t filtersLen, const Dom *dom,
                          Uint16HashSet *set);

QueryStatus
getFilteredAdjacents(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                     size_t filtersLen, const Dom *dom, size_t numberOfSiblings,
                     Uint16HashSet *set);
QueryStatus
getFilteredDescendants(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                       size_t filtersLen, const Dom *dom, size_t depth,
                       Uint16HashSet *set);
#endif
