#ifndef FLO_HTML_PARSER_DEFINITIONS_H
#define FLO_HTML_PARSER_DEFINITIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "flo/html-parser/util/array.h"
#include "flo/html-parser/util/text/string.h"

// TODO: when we do containsStringHashSet we convert from a ptrdiff_t to this
// type. Maybe we have to create a new string-type that accepts a type that
// decides what the max capacity is based on the type of the entry index and the
// capacity.
typedef uint16_t flo_html_index_id;

typedef uint16_t flo_html_node_id;

typedef FLO_HTML_DYNAMIC_ARRAY(flo_html_String) flo_html_String_d_a;
#ifdef __cplusplus
}
#endif

#endif
