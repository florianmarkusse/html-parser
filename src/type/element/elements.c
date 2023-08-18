#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/type/data/data-functions.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/utils/print/error.h"

ElementStatus createDataContainer(DataContainer *dataContainer) {
    dataContainer->tags.pairedLen = 0;
    dataContainer->tags.singleLen = 0;
    dataContainer->tags.container.pageLen = 0;
    dataContainer->tags.container.pageSize = TAGS_PAGE_SIZE;

    dataContainer->propKeys.pairedLen = 0;
    dataContainer->propKeys.singleLen = 0;
    dataContainer->propKeys.container.pageLen = 0;
    dataContainer->propKeys.container.pageSize = PROP_KEYS_PAGE_SIZE;

    dataContainer->propValues.len = 0;
    dataContainer->propValues.container.pageLen = 0;
    dataContainer->propValues.container.pageSize = PROP_VALUES_PAGE_SIZE;

    dataContainer->text.len = 0;
    dataContainer->text.container.pageLen = 0;
    dataContainer->text.container.pageSize = TEXT_PAGE_SIZE;

    return ELEMENT_SUCCESS;
}

void destroyElementsContainer(ElementsContainer *container) {
    for (int i = 0; i < container->pageLen; i++) {
        free(container->pages[i].start);
    }
    container->pageLen = 0;
}

void destroyDataContainer(DataContainer *dataContainer) {
    destroyElementsContainer(&dataContainer->tags.container);
    dataContainer->tags.pairedLen = 0;
    dataContainer->tags.singleLen = 0;

    destroyElementsContainer(&dataContainer->propKeys.container);
    dataContainer->propKeys.pairedLen = 0;
    dataContainer->propKeys.singleLen = 0;

    destroyElementsContainer(&dataContainer->propValues.container);
    dataContainer->propValues.len = 0;

    destroyElementsContainer(&dataContainer->text.container);
    dataContainer->text.len = 0;
}

ElementStatus elementSizeCheck(char *buffer, const size_t bufferLen,
                               const char *elementStart,
                               const size_t elementLength) {
    if (elementLength >= bufferLen) {
        PRINT_ERROR("Element is too long, size=%zu, to fit into page.\n",
                    elementLength);
        PRINT_ERROR("Printing first part of element:\n");

        memcpy(buffer, elementStart, bufferLen - 1);
        buffer[bufferLen - 1] = '\0';

        PRINT_ERROR("%s\n", buffer);
        PRINT_ERROR("max size is %zu (null terminator :))\n", bufferLen - 1);

        return ELEMENT_TOO_LONG;
    }

    return ELEMENT_SUCCESS;
}

ElementStatus createElement(ElementsContainer *container, const char *element,
                            element_id *currentElementsLen,
                            const element_id offsetMask,
                            element_id *elementID) {
    if (*currentElementsLen >= POSSIBLE_ELEMENTS) {
        PRINT_ERROR("Max size of elements array is reached: %u\n",
                    POSSIBLE_ELEMENTS);
        PRINT_ERROR("Could not insert \"%s\".\n", element);
        return ELEMENT_ARRAY_FULL;
    }

    DataPageStatus dataPageStatus =
        insertIntoPage(element, strlen(element) + 1, TOTAL_PAGES,
                       offsetMask | (*currentElementsLen), container);
    if (dataPageStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(dataPageStatus),
                               "Could not find or create element \"%s\"",
                               element);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    *elementID = (offsetMask | (*currentElementsLen));
    (*currentElementsLen)++;

    return ELEMENT_SUCCESS;
}

ElementStatus findElement(const ElementsContainer *container,
                          const element_id *currentElementLen,
                          const char *elementName, element_id offsetMask,
                          element_id *elementID) {
    for (element_id i = offsetMask; i < (offsetMask | *currentElementLen);
         ++i) {
        if (strcmp(container->elements[i], elementName) == 0) {
            *elementID = i;
            return ELEMENT_SUCCESS;
        }
    }

    return ELEMENT_NOT_FOUND_OR_CREATED;
}

ElementStatus findOrCreateElement(ElementsContainer *container,
                                  const char *elementName,
                                  element_id *currentElementLen,
                                  const element_id offsetMask,
                                  element_id *elementID) {
    if (findElement(container, currentElementLen, elementName, offsetMask,
                    elementID) == ELEMENT_SUCCESS) {
        return ELEMENT_SUCCESS;
    }

    return createElement(container, elementName, currentElementLen, offsetMask,
                         elementID);
}

ElementStatus textElementToIndex(element_id *elementID) {
    *elementID = TEXT_OFFSET;
    return ELEMENT_SUCCESS;
}

ElementStatus
elementToIndex(ElementsContainer *container, element_id *currentElementLen,
               const char *elementStart, const size_t elementLength,
               const unsigned char isPaired, const unsigned char searchElements,
               element_id *elementID) {
    char buffer[container->pageSize];
    const ElementStatus sizeCheck = elementSizeCheck(
        buffer, container->pageSize, elementStart, elementLength);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    memcpy(buffer, elementStart, elementLength);
    buffer[elementLength] = '\0';

    if (searchElements) {
        return findOrCreateElement(container, buffer, currentElementLen,
                                   (isPaired ? 0 : SINGLES_OFFSET), elementID);
    }
    return createElement(container, buffer, currentElementLen,
                         (isPaired ? 0 : SINGLES_OFFSET), elementID);
}

unsigned char isSingle(const element_id index) {
    return (index >> SINGLES_MASK) != 0;
}

unsigned char isText(const element_id index) {
    return (index >> TEXT_MASK) != 0;
}

void printElements(const size_t currentLen,
                   char *const elements[TOTAL_ELEMENTS],
                   const size_t offsetMask) {
    printf("capacity: %zu/%u\n", currentLen, POSSIBLE_ELEMENTS);
    for (size_t i = offsetMask; i < (offsetMask | currentLen); i++) {
        printf("element ID: %-7zuelement: %-20s\n", i, elements[i]);
    }
    printf("\n\n");
}

void printElementPages(const ElementsContainer *container) {
    printf("element pages...\n");
    printf("%-15s: %hhu\n", "pages length", container->pageLen);
    for (size_t i = 0; i < container->pageLen; i++) {
        printf("%-15s: %lu\n", "space left", container->pages[i].spaceLeft);

        int printedChars = 0;
        char *copy = container->pages[i].start;
        while (printedChars < container->pageSize) {
            if (*copy == '\0') {
                printf("~");
            } else {
                printf("%c", *copy);
            }
            copy++;
            printedChars++;
        }
        printf("\n");
    }

    printf("\n\n");
}

void printElementStatus(const Elements *global) {
    printf("elements...\n");
    printElements(global->len, global->container.elements, 0);

    printElementPages(&global->container);
}

void printCombinedElementStatus(const CombinedElements *global) {
    printf("single elements...\n");
    printElements(global->singleLen, global->container.elements,
                  SINGLES_OFFSET);

    printf("paired elements...\n");
    printElements(global->pairedLen, global->container.elements, 0);

    printElementPages(&global->container);
}

void printTagStatus(DataContainer *dataContainer) {
    printf("printing tag status...\n\n");
    printCombinedElementStatus(&dataContainer->tags);
}

void printAttributeStatus(DataContainer *dataContainer) {
    printf("printing property status...\n\n");
    printCombinedElementStatus(&dataContainer->propKeys);
    printElementStatus(&dataContainer->propValues);
}

void printTextStatus(DataContainer *dataContainer) {
    printf("printing text status...\n\n");
    printElementStatus(&dataContainer->text);
}
