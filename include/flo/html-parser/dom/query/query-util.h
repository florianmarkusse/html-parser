#ifndef FLO_HTML_PARSER_DOM_QUERY_QUERY_UTIL_H
#define FLO_HTML_PARSER_DOM_QUERY_QUERY_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/dom/dom.h"
#include "msi-uint16.h"
#include "query-status.h"
#include "query.h"

#define FLO_HTML_INITIAL_QUERY_CAP (1U << 6U)
#define FLO_HTML_MAX_FILTERS_PER_ELEMENT (1U << 3U)

typedef enum {
    TAG,
    ALL_NODES,
    BOOLEAN_PROPERTY,
    PROPERTY
} flo_html_AttributeSelector;

typedef enum {
    NO_COMBINATOR,   // The first part of a query is done without a combinator
    ADJACENT,        // '+'
    CHILD,           // '>'
    GENERAL_SIBLING, // '~'
    DESCENDANT,      // ' ', Default combinator
} flo_html_Combinator;

typedef struct {
    flo_html_AttributeSelector attributeSelector;
    union {
        flo_html_index_id tagID;
        flo_html_index_id propID;
        struct {
            flo_html_index_id keyID;
            flo_html_index_id valueID;
        } keyValuePair;
    } data;
} flo_html_FilterType;

bool flo_html_filterNode(flo_html_node_id nodeID, flo_html_FilterType *filters,
                         ptrdiff_t filterslen, flo_html_Dom *dom);

bool flo_html_getNodesWithoutflo_html_Combinator(
    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    ptrdiff_t filtersLen, flo_html_Dom *dom, flo_msi_Uint16 *set,
    flo_Arena *perm);

flo_html_QueryStatus flo_html_getFilteredAdjacents(
    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    ptrdiff_t filtersLen, flo_html_Dom *dom, ptrdiff_t numberOfSiblings,
    flo_msi_Uint16 *set, flo_Arena *perm);
flo_html_QueryStatus flo_html_getFilteredDescendants(
    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    ptrdiff_t filtersLen, flo_html_Dom *dom, ptrdiff_t depth,
    flo_msi_Uint16 *set, flo_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
