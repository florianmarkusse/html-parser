#ifndef FLO_HTML_PARSER_DEFINITIONS_H
#define FLO_HTML_PARSER_DEFINITIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "array.h"
#include "text/string.h"

// TODO: can we get rid of this???
typedef uint16_t flo_html_index_id;
#define FLO_HTML_MAX_INDEX_ID UINT16_MAX

typedef uint16_t flo_html_node_id;
#define FLO_HTML_MAX_NODE_ID UINT16_MAX

#ifdef __cplusplus
}
#endif

#endif
