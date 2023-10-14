#include <stdlib.h>

#include "flo/html-parser/type/data/data-page.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

flo_html_DataPage flo_html_initDataPage(const ptrdiff_t pageSize,
                                        flo_html_Arena *perm) {
    char *data = FLO_HTML_NEW(perm, char, pageSize);
    return (flo_html_DataPage){
        .start = data, .freeSpace = data, .spaceLeft = pageSize};
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
