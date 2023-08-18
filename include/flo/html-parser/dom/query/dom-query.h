#ifndef FLO_HTML_PARSER_DOM_QUERY_DOM_QUERY_H
#define FLO_HTML_PARSER_DOM_QUERY_DOM_QUERY_H

#include "dom-query-status.h"
#include "flo/html-parser/dom/dom.h"

#define INITIAL_QUERY_CAP (1U << 6U)
#define MAX_FILTERS_PER_ELEMENT (1U << 3U)

typedef enum { TAG, BOOLEAN_PROPERTY, PROPERTY } AttributeSelector;

typedef enum {
    NO_COMBINATOR,   // The first element is selected without a combinator
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

QueryStatus querySelectorAll(const char *cssQuery, const Dom *dom,
                             const DataContainer *dataContainer,
                             node_id **results, size_t *resultsLen);

#endif