#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/hash/string-hash.h"
#include "flo/html-parser/type/data/data-functions.h"
#include "flo/html-parser/type/data/data-page.h"
#include "flo/html-parser/utils/print/error.h"

DataPage createDataPage(const size_t pageSize) {
    DataPage dataPage;
    dataPage.start = malloc(pageSize);
    dataPage.freeSpace = dataPage.start;
    dataPage.spaceLeft = pageSize;

    return dataPage;
}

// DataPageStatus insertIntoPage(const void *data, const size_t byteLen,
//                               const size_t totalPages,
//                               const size_t elementIndex,
//                               ElementsContainer *container) {
//     page_id suitableIndex = container->pageLen;
//     for (page_id i = 0; i < container->pageLen; ++i) {
//         if (container->pages[i].spaceLeft >= byteLen) {
//             suitableIndex = i;
//             break;
//         }
//     }
//
//     if (suitableIndex == container->pageLen) {
//         if (container->pageLen < totalPages) {
//             container->pages[suitableIndex] =
//                 createDataPage(container->pageSize);
//             if (container->pages[suitableIndex].start == NULL) {
//                 PRINT_ERROR("Failed to allocate memory for new tag page.\n");
//                 return DATA_PAGE_ERROR_MEMORY;
//             }
//             container->pageLen++;
//         } else {
//             PRINT_ERROR("No more capacity to create new pages.\n");
//             PRINT_ERROR("All %zu page(s) of %zu bytes are full.\n",
//             totalPages,
//                         container->pageSize);
//             return DATA_PAGE_NO_CAPACITY;
//         }
//     }
//
//     container->elements[elementIndex] =
//         container->pages[suitableIndex].freeSpace;
//     memcpy(container->pages[suitableIndex].freeSpace, data, byteLen);
//     container->pages[suitableIndex].freeSpace += byteLen;
//     container->pages[suitableIndex].spaceLeft -= byteLen;
//
//     return DATA_PAGE_SUCCESS;
// }

DataPageStatus newInsertIntoPage(const void *data, const size_t byteLen,
                                 const size_t totalPages,
                                 StringRegistry *newElements,
                                 HashElement *hashElement, indexID *indexID) {
    ElementsContainer *container = &newElements->container;
    page_id suitableIndex = container->pageLen;
    for (page_id i = 0; i < container->pageLen; ++i) {
        if (container->pages[i].spaceLeft >= byteLen) {
            suitableIndex = i;
            break;
        }
    }

    if (suitableIndex == container->pageLen) {
        if (container->pageLen < totalPages) {
            container->pages[suitableIndex] =
                createDataPage(container->pageSize);
            if (container->pages[suitableIndex].start == NULL) {
                PRINT_ERROR("Failed to allocate memory for new tag page.\n");
                return DATA_PAGE_ERROR_MEMORY;
            }
            container->pageLen++;
        } else {
            PRINT_ERROR("No more capacity to create new pages.\n");
            PRINT_ERROR("All %zu page(s) of %zu bytes are full.\n", totalPages,
                        container->pageSize);
            return DATA_PAGE_NO_CAPACITY;
        }
    }

    memcpy(container->pages[suitableIndex].freeSpace, data, byteLen);
    insertStringWithDataHashSet(&newElements->set,
                                container->pages[suitableIndex].freeSpace,
                                hashElement, indexID);
    container->pages[suitableIndex].freeSpace += byteLen;
    container->pages[suitableIndex].spaceLeft -= byteLen;

    return DATA_PAGE_SUCCESS;
}
