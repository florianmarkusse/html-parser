#ifndef FLO_HTML_PARSER_DOM_QUERYING_DOCUMENT_QUERYING_STATUS_H
#define FLO_HTML_PARSER_DOM_QUERYING_DOCUMENT_QUERYING_STATUS_H

typedef enum {
    QUERYING_SUCCESS,
    QUERYING_INVALID_ELEMENT,
    QUERYING_INVALID_COMBINATOR,
    QUERYING_TOO_MANY_ELEMENT_FILTERS,
    QUERYING_NOT_FOUND,
    QUERYING_MEMORY_ERROR,
    QUERYING_INITIALIZATION_ERROR,
    QUERYING_NUM_STATUS
} QueryingStatus;

static const char *const QueryingStatusStrings[QUERYING_NUM_STATUS] = {
    "Success",
    "Invalid element",
    "Invalid combinator",
    "Too many filters for a single element",
    "Detected unforeseen tag or attribute",
    "Memory error",
    "Initialization error"};

__attribute__((unused)) static const char *
queryingStatusToString(QueryingStatus status) {
    if (status >= 0 && status < QUERYING_NUM_STATUS) {
        return QueryingStatusStrings[status];
    }
    return "Unknown querying status code!";
}

#endif
