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

#define INITIAL_QUERY_CAP (1U << 6U)
#define MAX_FILTERS_PER_ELEMENT (1U << 3U)

typedef enum { TAG, ALL_NODES, BOOLEAN_PROPERTY, PROPERTY } AttributeSelector;

typedef enum {
    NO_COMBINATOR,   // The first part of a query is done without a combinator
    ADJACENT,        // '+'
    CHILD,           // '>'
    GENERAL_SIBLING, // '~'
    DESCENDANT,      // ' ', Default combinator
    NUM_COMBINATORS,
} Combinator;

typedef struct {
    AttributeSelector attributeSelector;
    union {
        element_id tagID;
        element_id propID;
        struct {
            element_id keyID;
            element_id valueID;
        } __attribute__((aligned(4))) keyValuePair;
    } data;
} __attribute__((aligned(8))) FilterType;

bool filterNode(node_id nodeID, const FilterType *filters, size_t filterslen,
                const Dom *dom);
indexID getTagID(const char *tag, const TextStore *textStore);
indexID getBoolPropID(const char *boolProp, const TextStore *textStore);
indexID getPropKeyID(const char *keyProp, const TextStore *textStore);
indexID getPropValueID(const char *valueProp,
                       const TextStore *textStore);

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
