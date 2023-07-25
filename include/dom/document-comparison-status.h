#ifndef DOM_DOCUMENT_COMPARISON_STATUS_H
#define DOM_DOCUMENT_COMPARISON_STATUS_H

typedef enum {
    COMPARISON_SUCCESS,
    COMPARISON_DIFFERENT_TAGS,
    COMPARISON_NUM_STATUS
} ComparisonStatus;

static const char *const ComparisonStatusStrings[COMPARISON_NUM_STATUS] = {
    "Success", "Different type of tags"};

static const char *comparisonStatusToString(ComparisonStatus status) {
    if (status >= 0 && status < COMPARISON_NUM_STATUS) {
        return ComparisonStatusStrings[status];
    }
    return "Unknown comparison status code!";
}

#endif
