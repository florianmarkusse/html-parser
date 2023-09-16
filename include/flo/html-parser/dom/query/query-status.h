#ifndef FLO_HTML_PARSER_DOM_QUERY_QUERY_STATUS_H
#define FLO_HTML_PARSER_DOM_QUERY_QUERY_STATUS_H

typedef enum {
    QUERY_SUCCESS,
    QUERY_INVALID_ELEMENT,
    QUERY_INVALID_COMBINATOR,
    QUERY_TOO_MANY_ELEMENT_FILTERS,
    QUERY_NOT_SEEN_BEFORE,
    QUERY_MEMORY_ERROR,
    QUERY_INITIALIZATION_ERROR,
    QUERY_NUM_STATUS
} flo_html_QueryStatus;

static const char *const queryStatusStrings[QUERY_NUM_STATUS] = {
    "Success",
    "Invalid element",
    "Invalid combinator",
    "Too many filters for a single element",
    "Detected unforeseen tag or attribute",
    "Memory error",
    "Initialization error"};

__attribute__((unused)) static const char *
flo_html_queryingStatusToString(flo_html_QueryStatus status) {
    if (status >= 0 && status < QUERY_NUM_STATUS) {
        return queryStatusStrings[status];
    }
    return "Unknown query status code!";
}

#endif
