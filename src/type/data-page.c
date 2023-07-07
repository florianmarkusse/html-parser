#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type/data-page.h"
#include "utils/print/error.h"

DataPage createDataPage() {
    DataPage dataPage;
    dataPage.start = malloc(PAGE_SIZE);
    dataPage.freeSpace = dataPage.start;
    dataPage.spaceLeft = PAGE_SIZE;

    return dataPage;
}

DataPageStatus insertIntoPage(const void *data, size_t byteLen, DataPage *pages,
                              size_t totalPages, page_id *pageLen,
                              void **address) {
    // Ensure tag fits into a page.
    if (byteLen > PAGE_SIZE) {
        PRINT_ERROR("data is too long for page.\n");
        PRINT_ERROR("\n------------------------\n");
        PRINT_ERROR("%s", (char *)data);
        PRINT_ERROR("\n------------------------\n");
        PRINT_ERROR("Data size:\t%zu\tPage size:\t%u\n", byteLen, PAGE_SIZE);
        return DATA_PAGE_DATA_TOO_LONG;
    }

    page_id suitableIndex = *pageLen;
    for (page_id i = 0; i < *pageLen; ++i) {
        DataPage *page = &(pages[i]);
        if (page->spaceLeft >= byteLen) {
            suitableIndex = i;
            break;
        }
    }

    if (suitableIndex == *pageLen) {
        if (*pageLen < totalPages) {
            pages[suitableIndex] = createDataPage();
            if (pages[suitableIndex].start == NULL) {
                PRINT_ERROR("Failed to allocate memory for new tag page.\n");
                return DATA_PAGE_ERROR_MEMORY;
            }
            (*pageLen)++;
        } else {
            PRINT_ERROR("No more capacity to create new tag pages.\n");
            PRINT_ERROR("All %zu page(s) of %u bytes are full.\n", totalPages,
                        PAGE_SIZE);
            return DATA_PAGE_NO_CAPACITY;
        }
    }

    // Duplicate the tag memory within the suitable page
    char *duplicatedTag = pages[suitableIndex].freeSpace;
    memcpy(duplicatedTag, data, byteLen);
    pages[suitableIndex].freeSpace += byteLen;
    pages[suitableIndex].spaceLeft -= byteLen;

    *address = duplicatedTag;
    return DATA_PAGE_SUCCESS;
}
