#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/hash/string-hash.h"
#include "flo/html-parser/type/data/data-functions.h"
#include "flo/html-parser/type/data/data-page.h"
#include "flo/html-parser/utils/print/error.h"

DataPageStatus createDataPage(DataPage *dataPage, const size_t pageSize) {
    dataPage->start = malloc(pageSize);
    if (dataPage->start == NULL) {
        PRINT_ERROR("Failed to allocate memory for new tag page.\n");
        return DATA_PAGE_ERROR_MEMORY;
    }
    dataPage->freeSpace = dataPage->start;
    dataPage->spaceLeft = pageSize;

    return DATA_PAGE_SUCCESS;
}

DataPageStatus insertInSuitablePage(const void *data, const size_t byteLen,
                                    const size_t totalPages,
                                    ElementsContainer *container,
                                    char **dataLocation) {
    DataPageStatus status = DATA_PAGE_SUCCESS;
    page_id index = container->pageLen;
    for (page_id i = 0; i < container->pageLen; ++i) {
        if (container->pages[i].spaceLeft >= byteLen) {
            index = i;
            break;
        }
    }

    if (index == container->pageLen) {
        if (container->pageLen < totalPages) {
            DataPage dataPage;
            if ((status = createDataPage(&dataPage, container->pageSize)) !=
                DATA_PAGE_SUCCESS) {
                return status;
            }
            container->pages[index] = dataPage;
            container->pageLen++;
        } else {
            PRINT_ERROR("No more capacity to create new pages.\n");
            PRINT_ERROR("All %zu page(s) of %zu bytes are full.\n", totalPages,
                        container->pageSize);
            return DATA_PAGE_NO_CAPACITY;
        }
    }

    memcpy(container->pages[index].freeSpace, data, byteLen);
    *dataLocation = container->pages[index].freeSpace;
    container->pages[index].freeSpace += byteLen;
    container->pages[index].spaceLeft -= byteLen;

    return status;
}

DataPageStatus insertIntoPageWithhash(const void *data, const size_t byteLen,
                                      const size_t totalPages,
                                      StringRegistry *stringRegistry,
                                      HashElement *hashElement,
                                      indexID *indexID) {
    DataPageStatus status = DATA_PAGE_SUCCESS;

    ElementsContainer *container = &stringRegistry->container;
    char *dataLocation = NULL;
    if ((status = insertInSuitablePage(data, byteLen, totalPages, container,
                                       &dataLocation)) != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(status),
                               "Failed to insert data in suitable page %s\n",
                               (char *)data);
        return status;
    }

    insertStringAtHash(&stringRegistry->set, dataLocation, hashElement,
                       indexID);

    return status;
}
