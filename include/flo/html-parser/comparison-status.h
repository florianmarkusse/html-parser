#ifndef FLO_HTML_PARSER_COMPARISON_STATUS_H
#define FLO_HTML_PARSER_COMPARISON_STATUS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/util/text/string.h"

typedef enum {
    COMPARISON_SUCCESS,
    COMPARISON_MEMORY,
    COMPARISON_DIFFERENT_NODE_TYPE,
    COMPARISON_DIFFERENT_SIZES,
    COMPARISON_DIFFERENT_CONTENT,
    COMPARISON_NUM_STATUS
} flo_html_ComparisonStatus;

flo_String flo_html_comparisonStatusToString(flo_html_ComparisonStatus status);

#ifdef __cplusplus
}
#endif

#endif
