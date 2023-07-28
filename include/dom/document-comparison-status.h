#ifndef DOM_DOCUMENT_COMPARISON_STATUS_H
#define DOM_DOCUMENT_COMPARISON_STATUS_H

typedef enum {
    COMPARISON_SUCCESS,
    COMPARISON_DIFFERENT_NODE_TYPE,
    COMPARISON_DIFFERENT_TAGS,
    COMPARISON_MISSING_PROPERTIES,
    COMPARISON_DIFFERENT_PROPERTIES,
    COMPARISON_DIFFERENT_SIZES,
    COMPARISON_DIFFERENT_TEXT,
    COMPARISON_NUM_STATUS
} ComparisonStatus;

static const char *const ComparisonStatusStrings[COMPARISON_NUM_STATUS] = {
    "Success",
    "Different type of node",
    "Different tags",
    "Nodes have different number of properties",
    "Nodes have different properties",
    "Different number of nodes",
    "Text nodes have different content"};

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
