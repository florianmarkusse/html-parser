#include <stdlib.h>

#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/utils/memory/memory.h"

ElementStatus initElementsContainer(ElementsContainer *elementsContainer,
                                    const size_t pageSize) {
    elementsContainer->pages = malloc(sizeof(DataPage) * TOTAL_PAGES);

    if (elementsContainer->pages == NULL) {
        FREE_TO_NULL(elementsContainer->pages);
        return ELEMENT_MEMORY;
    }

    elementsContainer->pageLen = 0;
    elementsContainer->pageSize = pageSize;

    return ELEMENT_SUCCESS;
}

void destroyElementsContainer(ElementsContainer *elementsContainer) {
    for (int i = 0; i < elementsContainer->pageLen; i++) {
        FREE_TO_NULL(elementsContainer->pages[i].start);
    }
    FREE_TO_NULL(elementsContainer->pages);
    elementsContainer->pageLen = 0;
}
