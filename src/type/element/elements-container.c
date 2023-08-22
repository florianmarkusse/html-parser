#include <stdlib.h>

#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/utils/memory/memory.h"

ElementStatus initElementsContainer(ElementsContainer *elementsContainer,
                                    const size_t pageSize) {
    elementsContainer->elements = malloc(sizeof(char *) * TOTAL_ELEMENTS);
    elementsContainer->pages = malloc(sizeof(DataPage) * TOTAL_PAGES);

    if (elementsContainer->elements == NULL ||
        elementsContainer->pages == NULL) {
        FREE_TO_NULL(elementsContainer->elements);
        FREE_TO_NULL(elementsContainer->pages);
        return ELEMENT_MEMORY;
    }

    elementsContainer->pageLen = 0;
    elementsContainer->pageSize = pageSize;

    return ELEMENT_SUCCESS;
}

void destroyElementsContainer(ElementsContainer *elementsContainer) {
    FREE_TO_NULL(elementsContainer->elements);
    for (int i = 0; i < elementsContainer->pageLen; i++) {
        FREE_TO_NULL(elementsContainer->pages[i].start);
    }
    FREE_TO_NULL(elementsContainer->pages);
    elementsContainer->pages = NULL;
    elementsContainer->pageLen = 0;
}
