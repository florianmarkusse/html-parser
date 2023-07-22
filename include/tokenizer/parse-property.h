#ifndef TOKENIZER_PARSE_PROPERTY_H
#define TOKENIZER_PARSE_PROPERTY_H

#include <stddef.h>

typedef struct {
    const char *start;
    size_t len;
} __attribute__((aligned(16))) ParseProperty;

#endif
