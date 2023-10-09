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

unsigned char *flo_html_insertIntoPage(const flo_html_String data,
                                       flo_html_DataPage *page) {
    // TODO: dynamic
    if (page->spaceLeft < data.len) {
        FLO_HTML_PRINT_ERROR("No more capacity to add data to page.\n");
    }

    memcpy(page->freeSpace, data.buf, data.len);
    unsigned char *dataLocation = page->freeSpace;
    page->freeSpace += data.len;
    page->spaceLeft -= data.len;

    return dataLocation;
}

flo_html_index_id flo_html_insertIntoPageWithHash(
    const flo_html_String data, flo_html_DataPage *page,
    flo_html_StringHashSet *set, flo_html_HashElement *hashElement) {
    unsigned char *dataLocation = flo_html_insertIntoPage(data, page);
    return flo_html_insertStringAtHash(
        set, FLO_HTML_S_LEN(dataLocation, data.len), hashElement);
}
