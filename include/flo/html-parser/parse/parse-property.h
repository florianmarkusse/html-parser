#ifndef FLO_HTML_PARSER_PARSE_PARSE_PROPERTY_H
#define FLO_HTML_PARSER_PARSE_PARSE_PROPERTY_H

#include <stddef.h>

typedef struct {
    const char *start;
    size_t len;
} __attribute__((aligned(16))) ParseProperty;

#endif
