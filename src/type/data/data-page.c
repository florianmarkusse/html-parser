#include <stdlib.h>

#include "flo/html-parser/type/data/data-page.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

void flo_html_initDataPage(flo_html_DataPage *dataPage,
                           const ptrdiff_t pageSize, flo_html_Arena *perm) {
    dataPage->start = FLO_HTML_NEW(perm, char, pageSize);
    dataPage->freeSpace = dataPage->start;
    dataPage->spaceLeft = pageSize;
}

void flo_html_destroyDataPage(flo_html_DataPage *dataPage) {
    FLO_HTML_FREE_TO_NULL(dataPage->start);
    dataPage->spaceLeft = 0;
    dataPage->freeSpace = NULL;
}

flo_html_DataPageStatus flo_html_insertIntoPage(const flo_html_String data,
                                                flo_html_DataPage *page,
                                                char **dataLocation) {
    if (page->spaceLeft < data.len) {
        FLO_HTML_PRINT_ERROR("No more capacity to add data to page.\n");
        return DATA_PAGE_NO_CAPACITY;
    }

    memcpy(page->freeSpace, data.buf, data.len);
    *dataLocation = page->freeSpace;
    page->freeSpace += data.len;
    page->spaceLeft -= data.len;

    return DATA_PAGE_SUCCESS;
}

flo_html_DataPageStatus flo_html_insertIntoPageWithHash(
    const flo_html_String data, flo_html_DataPage *page,
    flo_html_StringHashSet *set, flo_html_HashElement *hashElement,
    flo_html_indexID *indexID) {
    flo_html_DataPageStatus status = DATA_PAGE_SUCCESS;

    char *dataLocation = NULL;
    if ((status = flo_html_insertIntoPage(data, page, &dataLocation)) !=
        DATA_PAGE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(flo_html_dataPageStatusToString(status),
                                        "Failed to insert data page %.*s\n",
                                        FLO_HTML_S_P(data));
        return status;
    }

    flo_html_insertStringAtHash(set, FLO_HTML_S_LEN(dataLocation, data.len),
                                hashElement, indexID);

    return status;
}
