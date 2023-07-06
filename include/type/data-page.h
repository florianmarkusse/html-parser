#ifndef TYPE_DATA_PAGE_H
#define TYPE_DATA_PAGE_H

#include <stddef.h>
#include <stdint.h>

#include "data-page-status.h"

#define PAGE_SIZE (1U << 10U)
typedef uint16_t page_space;
#define TOTAL_PAGES (1U << 4U)
typedef uint8_t page_id;

typedef struct {
    char *start;
    char *freeSpace;
    page_space spaceLeft;
} __attribute__((aligned(32))) DataPage;

// Left up to the client to check if allocation was succesful.
// start == NULL
DataPage createDataPage();
DataPageStatus insertIntoPage(const void *data, size_t byteLen,
                              DataPage pages[TOTAL_PAGES], page_id *pageLen,
                              void *address);
#endif
