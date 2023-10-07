#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/type/data/data-functions.h"
#include "flo/html-parser/type/data/data-page.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/hash/string-hash.h"

flo_html_DataPageStatus createflo_html_DataPage(flo_html_DataPage *dataPage,
                                                const ptrdiff_t pageSize) {
    dataPage->start = malloc(pageSize);
    if (dataPage->start == NULL) {
        FLO_HTML_PRINT_ERROR("Failed to allocate memory for new tag page.\n");
        return DATA_PAGE_ERROR_MEMORY;
    }
    dataPage->freeSpace = dataPage->start;
    dataPage->spaceLeft = pageSize;

    return DATA_PAGE_SUCCESS;
}

flo_html_DataPageStatus flo_html_insertIntoSuitablePage(
    const flo_html_String data, const ptrdiff_t totalPages,
    flo_html_ElementsContainer *container, char **dataLocation) {
    flo_html_DataPageStatus status = DATA_PAGE_SUCCESS;
    ptrdiff_t index = container->pageLen;
    for (ptrdiff_t i = 0; i < container->pageLen; ++i) {
        if (container->pages[i].spaceLeft >= data.len) {
            index = i;
            break;
        }
    }

    if (index == container->pageLen) {
        if (container->pageLen < totalPages) {
            flo_html_DataPage dataPage;
            if ((status = createflo_html_DataPage(
                     &dataPage, container->pageSize)) != DATA_PAGE_SUCCESS) {
                return status;
            }
            container->pages[index] = dataPage;
            container->pageLen++;
        } else {
            FLO_HTML_PRINT_ERROR("No more capacity to create new pages.\n");
            FLO_HTML_PRINT_ERROR("All %zu page(s) of %zu bytes are full.\n",
                                 totalPages, container->pageSize);
            return DATA_PAGE_NO_CAPACITY;
        }
    }

    memcpy(container->pages[index].freeSpace, data.buf, data.len);
    *dataLocation = container->pages[index].freeSpace;
    container->pages[index].freeSpace += data.len;
    container->pages[index].spaceLeft -= data.len;

    return status;
}

flo_html_DataPageStatus flo_html_insertIntoPageWithHash(
    const flo_html_String data, const ptrdiff_t totalPages,
    flo_html_StringRegistry *stringRegistry, flo_html_HashElement *hashElement,
    flo_html_indexID *flo_html_indexID) {
    flo_html_DataPageStatus status = DATA_PAGE_SUCCESS;

    flo_html_ElementsContainer *container = &stringRegistry->container;
    char *dataLocation = NULL;
    if ((status = flo_html_insertIntoSuitablePage(data, totalPages, container,
                                                  &dataLocation)) !=
        DATA_PAGE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(
            flo_html_dataPageStatusToString(status),
            "Failed to insert data in suitable page %.*s\n",
            FLO_HTML_S_P(data));
        return status;
    }

    flo_html_insertStringAtHash(&stringRegistry->set,
                                FLO_HTML_S_LEN(dataLocation, data.len),
                                hashElement, flo_html_indexID);

    return status;
}
