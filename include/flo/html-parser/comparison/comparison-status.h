#ifndef FLO_HTML_PARSER_COMPARISON_COMPARISON_STATUS_H
#define FLO_HTML_PARSER_COMPARISON_COMPARISON_STATUS_H

typedef enum {
    COMPARISON_SUCCESS,
    COMPARISON_MEMORY,
    COMPARISON_DIFFERENT_NODE_TYPE,
    COMPARISON_DIFFERENT_SIZES,
    COMPARISON_DIFFERENT_CONTENT,
    COMPARISON_NUM_STATUS
} ComparisonStatus;

static const char *const ComparisonStatusStrings[COMPARISON_NUM_STATUS] = {
    "Success", "Memory error", "Different type of node",
    "Collections have different sizes", "Collections have different content"};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static const char *
comparisonStatusToString(ComparisonStatus status) {
    if (status >= 0 && status < COMPARISON_NUM_STATUS) {
        return ComparisonStatusStrings[status];
    }
    return "Unknown comparison status code!";
}

#endif
