#ifndef TYPE_ELEMENT_ELEMENTS_CONTAINER_H
#define TYPE_ELEMENT_ELEMENTS_CONTAINER_H

#include <stddef.h>
#include <stdint.h>

#include "../data/data-page.h"

typedef uint16_t element_id;
#define TOTAL_ELEMENTS_NUM_BITS (sizeof(element_id) * 8)

#define EXPONENT 8U

#define SINGLES_MASK (EXPONENT - 2)
#define SINGLES_OFFSET (1U << SINGLES_MASK)

// The remaining bits are used as masks.
#define TOTAL_ELEMENTS (1U << EXPONENT)
#define POSSIBLE_ELEMENTS (1U << SINGLES_MASK)

#define TOTAL_PAGES (1U << 4U)

// The size of each page of each global.
#define TAGS_PAGE_SIZE (1U << 10U)
#define PROP_KEYS_PAGE_SIZE (1U << 10U)
#define PROP_VALUES_PAGE_SIZE (1U << 10U)
#define TEXT_PAGE_SIZE (1U << 12U)

typedef struct {
    char *elements[TOTAL_ELEMENTS];
    DataPage pages[TOTAL_PAGES];
    page_id pageLen;
    size_t pageSize;
} __attribute__((aligned(128))) ElementsContainer;

#endif
