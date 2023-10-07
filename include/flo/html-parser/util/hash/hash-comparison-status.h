#ifndef FLO_HTML_PARSER_UTIL_HASH_HASH_COMPARISON_STATUS_H
#define FLO_HTML_PARSER_UTIL_HASH_HASH_COMPARISON_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
    HASH_COMPARISON_SUCCESS,
    HASH_COMPARISON_DIFFERENT_SIZES,
    HASH_COMPARISON_DIFFERENT_CONTENT,
    HASH_COMPARISON_NUM_STATUS
} flo_html_HashComparisonStatus;

static const char
    *const hashComparisonStatusStrings[HASH_COMPARISON_NUM_STATUS] = {
        "Success", "Collections have different sizes",
        "Collections have different content"};

// Not always used, but very handy for those that actually do want readable
// error codes.
__attribute__((unused)) static const char *
flo_html_hashComparisonStatusToString(flo_html_HashComparisonStatus status) {
    if (status >= 0 && status < HASH_COMPARISON_NUM_STATUS) {
        return hashComparisonStatusStrings[status];
    }
    return "Unknown hash comparison status code!";
}

#ifdef __cplusplus
}
#endif

#endif
