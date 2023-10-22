#ifndef FLO_HTML_PARSER_COMPARISON_STATUS_H
#define FLO_HTML_PARSER_COMPARISON_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    COMPARISON_SUCCESS,
    COMPARISON_MEMORY,
    COMPARISON_DIFFERENT_NODE_TYPE,
    COMPARISON_DIFFERENT_SIZES,
    COMPARISON_DIFFERENT_CONTENT,
    COMPARISON_NUM_STATUS
} flo_html_ComparisonStatus;

static char *comparisonStatusStrings[COMPARISON_NUM_STATUS] = {
    "Success", "Memory error", "Different type of node",
    "Collections have different sizes", "Collections have different content"};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static char *
flo_html_comparisonStatusToString(flo_html_ComparisonStatus status) {
    if (status >= 0 && status < COMPARISON_NUM_STATUS) {
        return comparisonStatusStrings[status];
    }
    return "Unknown comparison status code!";
}

#ifdef __cplusplus
}
#endif

#endif
