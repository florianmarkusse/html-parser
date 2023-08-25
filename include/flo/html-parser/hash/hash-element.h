#ifndef FLO_HTML_PARSER_HASH_HASH_ELEMENT_H
#define FLO_HTML_PARSER_HASH_HASH_ELEMENT_H

#include <stddef.h>

typedef struct {
    size_t hash;
    unsigned char
        offset; // This offset is based on the linear probing tactic used. if
                // this changes, this will likely go very wrong.
} __attribute__((aligned(16))) HashElement;

#endif
