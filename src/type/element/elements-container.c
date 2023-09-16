#include <stdlib.h>

#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/utils/memory/memory.h"

flo_html_ElementStatus
initflo_html_ElementsContainer(flo_html_ElementsContainer *elementsContainer,
                               const size_t pageSize) {
    elementsContainer->pages =
        malloc(sizeof(flo_html_DataPage) * FLO_HTML_TOTAL_PAGES);

    if (elementsContainer->pages == NULL) {
        FLO_HTML_FREE_TO_NULL(elementsContainer->pages);
        return ELEMENT_MEMORY;
    }

    elementsContainer->pageLen = 0;
    elementsContainer->pageSize = pageSize;

    return ELEMENT_SUCCESS;
}

void destroyflo_html_ElementsContainer(
    flo_html_ElementsContainer *elementsContainer) {
    for (int i = 0; i < elementsContainer->pageLen; i++) {
        FLO_HTML_FREE_TO_NULL(elementsContainer->pages[i].start);
    }
    FLO_HTML_FREE_TO_NULL(elementsContainer->pages);
    elementsContainer->pageLen = 0;
}
