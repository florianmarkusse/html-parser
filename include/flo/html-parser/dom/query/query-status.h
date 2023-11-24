#ifndef FLO_HTML_PARSER_DOM_QUERY_QUERY_STATUS_H
#define FLO_HTML_PARSER_DOM_QUERY_QUERY_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "text/string.h"

typedef enum {
    QUERY_SUCCESS,
    QUERY_INVALID_ELEMENT,
    QUERY_TOO_MANY_ELEMENT_FILTERS,
    QUERY_NOT_SEEN_BEFORE,
    QUERY_MEMORY_ERROR,
    QUERY_NUM_STATUS
} flo_html_QueryStatus;

static flo_String queryStatusStrings[QUERY_NUM_STATUS] = {
    FLO_STRING("Success"),
    FLO_STRING("Invalid element"),
    FLO_STRING("Too many filters for a single element"),
    FLO_STRING("Detected unforeseen tag or attribute"),
    FLO_STRING("Memory error"),
};

__attribute__((unused)) static flo_String
flo_html_queryingStatusToString(flo_html_QueryStatus status) {
    if (status >= 0 && status < QUERY_NUM_STATUS) {
        return queryStatusStrings[status];
    }
    return FLO_STRING("Unknown query status code!");
}

#ifdef __cplusplus
}
#endif

#endif
