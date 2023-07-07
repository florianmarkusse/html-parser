#ifndef TYPE_DATA_PAGE_H
#define TYPE_DATA_PAGE_H

#include <stddef.h>
#include <stdint.h>

#include "data-page-status.h"

#define PAGE_SIZE (1U << 10U)
typedef uint16_t page_space;
typedef uint8_t page_id;

typedef struct {
    char *start;
    char *freeSpace;
    page_space spaceLeft;
} __attribute__((aligned(32))) DataPage;

DataPageStatus insertIntoPage(const void *data, size_t byteLen, DataPage *pages,
                              size_t totalPages, page_id *pageLen,
                              void **address);
#endif
