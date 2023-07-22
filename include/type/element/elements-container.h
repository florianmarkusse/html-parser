#ifndef TYPE_ELEMENT_ELEMENTS_CONTAINER_H
#define TYPE_ELEMENT_ELEMENTS_CONTAINER_H

#include "../data/data-page.h"

#define EXPONENT 10U
#define TOTAL_ELEMENTS (1U << EXPONENT)

#define TOTAL_PAGES (1U << 4U)

#define TAGS_PAGE_SIZE (1U << 10U)
#define PROP_KEYS_PAGE_SIZE (1U << 10U)
#define PROP_VALUES_PAGE_SIZE (1U << 10U)
#define TEXT_PAGE_SIZE (1U << 10U)

typedef struct {
    char *elements[TOTAL_ELEMENTS];
    DataPage pages[TOTAL_PAGES];
    page_id pageLen;
    size_t pageSize;
} __attribute__((aligned(128))) ElementsContainer;

#endif
