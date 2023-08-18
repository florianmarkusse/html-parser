#ifndef FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_CONTAINER_H
#define FLO_HTML_PARSER_TYPE_ELEMENT_ELEMENTS_CONTAINER_H

#include <stddef.h>
#include <stdint.h>

#include "flo/html-parser/type/data/data-page.h"

typedef uint16_t element_id;
#define TOTAL_ELEMENTS_NUM_BITS (sizeof(element_id) * 8)

#define EXPONENT 16U

// masks
#define TEXT_MASK (EXPONENT - 1)
#define TEXT_OFFSET (1U << TEXT_MASK)

#define SINGLES_MASK (EXPONENT - 2)
#define SINGLES_OFFSET (1U << SINGLES_MASK)

// The remaining bits are for the IDs.
#define TOTAL_ELEMENTS (1U << EXPONENT)
#define POSSIBLE_ELEMENTS (1U << SINGLES_MASK)

#define TOTAL_PAGES (1U << 8U)

// The size of each page of each global.
#define TAGS_PAGE_SIZE (1U << 12U)
#define PROP_KEYS_PAGE_SIZE (1U << 12U)
#define PROP_VALUES_PAGE_SIZE (1U << 14U)
#define TEXT_PAGE_SIZE (1U << 17U)

typedef struct {
    char *elements[TOTAL_ELEMENTS];
    DataPage pages[TOTAL_PAGES];
    page_id pageLen;
    size_t pageSize;
} __attribute__((aligned(128))) ElementsContainer;

#endif