#include "flo/html-parser/comparison-status.h"

static flo_String comparisonStatusStrings[COMPARISON_NUM_STATUS] = {
    FLO_STRING("Success"), FLO_STRING("Memory error"),
    FLO_STRING("Different type of node"),
    FLO_STRING("Collections have different sizes"),
    FLO_STRING("Collections have different content")};

flo_String flo_html_comparisonStatusToString(flo_html_ComparisonStatus status) {
    if (status >= 0 && status < COMPARISON_NUM_STATUS) {
        return comparisonStatusStrings[status];
    }
    return FLO_STRING("Unknown comparison status code!");
}
