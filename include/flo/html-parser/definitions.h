#ifndef FLO_HTML_PARSER_DEFINITIONS_H
#define FLO_HTML_PARSER_DEFINITIONS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

// TODO: when we do containsStringHashSet we convert from a ptrdiff_t to this
// type. Maybe we have to create a new string-type that accepts a type that
// decides what the max capacity is based on the type of the entry index and the
// capacity.
typedef uint16_t flo_html_index_id;

typedef uint64_t flo_html_page_space;
typedef uint8_t flo_html_page_id;

typedef uint16_t flo_html_node_id;

#ifdef __cplusplus
}
#endif

#endif
