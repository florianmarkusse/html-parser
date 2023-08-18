#ifndef FLO_HTML_PARSER_TYPE_DATA_DATA_PAGE_H
#define FLO_HTML_PARSER_TYPE_DATA_DATA_PAGE_H

#include <stddef.h>
#include <stdint.h>

#include "data-page-status.h"

typedef uint64_t page_space;
typedef uint8_t page_id;

typedef struct {
    char *start;
    char *freeSpace;
    page_space spaceLeft;
} __attribute__((aligned(32))) DataPage;

#endif
