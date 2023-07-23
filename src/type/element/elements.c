#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type/data/data-page.h"
#include "type/element/elements.h"
#include "utils/print/error.h"

CombinedElements gTags;
CombinedElements gPropKeys;
Elements gPropValues;
Elements gText;

ElementStatus createGlobals() {
    gTags.pairedLen = 0;
    gTags.singleLen = 0;
    gTags.container.pageLen = 0;
    gTags.container.pageSize = TAGS_PAGE_SIZE;

    gPropKeys.pairedLen = 0;
    gPropKeys.singleLen = 0;
    gPropKeys.container.pageLen = 0;
    gPropKeys.container.pageSize = PROP_KEYS_PAGE_SIZE;

    gPropValues.len = 0;
    gPropValues.container.pageLen = 0;
    gPropValues.container.pageSize = PROP_VALUES_PAGE_SIZE;

    gText.len = 0;
    gText.container.pageLen = 0;
    gText.container.pageSize = TEXT_PAGE_SIZE;

    return ELEMENT_SUCCESS;
}

void destroyElementsContainer(ElementsContainer *elements) {
    for (int i = 0; i < elements->pageLen; i++) {
        free(elements->pages[i].start);
    }
    elements->pageLen = 0;
}

void destroyGlobals() {
    destroyElementsContainer(&gTags.container);
    gTags.pairedLen = 0;
    gTags.singleLen = 0;

    destroyElementsContainer(&gPropKeys.container);
    gPropKeys.pairedLen = 0;
    gPropKeys.singleLen = 0;

    destroyElementsContainer(&gPropValues.container);
    gPropValues.len = 0;

    destroyElementsContainer(&gText.container);
    gText.len = 0;
}

ElementStatus createElement(ElementsContainer *container, const char *element,
                            element_id *currentElementsLen,
                            const element_id offsetMask,
                            element_id *elementID) {
    char *address = NULL;
    DataPageStatus dataPageStatus = insertIntoPage(
        element, strlen(element) + 1, container->pages, TOTAL_PAGES,
        &container->pageLen, container->pageSize, (void *)&address);
    if (dataPageStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(dataPageStatus),
                               "Could not find or create element \"%s\"",
                               element);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    container->elements[offsetMask | *currentElementsLen] = address;
    *elementID = offsetMask | (*currentElementsLen);
    (*currentElementsLen)++;

    return ELEMENT_SUCCESS;
}

ElementStatus findOrCreateElement(ElementsContainer *container,
                                  const char *elementName,
                                  element_id *currentElementLen,
                                  const element_id offsetMask,
                                  element_id *elementID) {
    printf("offsetMask: %u\n", offsetMask);
    printf("up until: %u\n", offsetMask | *currentElementLen);
    printf("\n\n");
    for (element_id i = offsetMask; i < (offsetMask | *currentElementLen);
         ++i) {
        printf("%s\n", container->elements[i]);
        printf("%s\n", elementName);
        if (strcmp(container->elements[i], elementName) == 0) {
            *elementID = i;
            return ELEMENT_SUCCESS;
        }
    }

    return createElement(container, elementName, currentElementLen, offsetMask,
                         elementID);
}

ElementStatus elementSizeCheck(char *buffer, const size_t bufferLen,
                               const char *elementStart,
                               const size_t elementLength) {
    if (elementLength >= bufferLen) {
        PRINT_ERROR("Tag is too long, size=%zu, to fit into page.\n",
                    elementLength);
        PRINT_ERROR("Printing first part of tag:\n");

        memcpy(buffer, elementStart, bufferLen - 1);
        buffer[bufferLen - 1] = '\0';

        PRINT_ERROR("%s\n", buffer);
        PRINT_ERROR("max size is %zu\n", bufferLen);

        return ELEMENT_TOO_LONG;
    }

    return ELEMENT_SUCCESS;
}

ElementStatus elementToIndex(Elements *global, const char *elementStart,
                             const size_t elementLength,
                             element_id *elementID) {
    char buffer[global->container.pageSize];
    const ElementStatus sizeCheck = elementSizeCheck(
        buffer, global->container.pageSize, elementStart, elementLength);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    memcpy(buffer, elementStart, elementLength);
    buffer[elementLength] = '\0';

    return findOrCreateElement(&global->container, buffer, &(global->len), 0,
                               elementID);
}

ElementStatus combinedElementToIndex(CombinedElements *global,
                                     const char *elementStart,
                                     const size_t elementLength,
                                     const unsigned char isPaired,
                                     element_id *elementID) {
    char buffer[global->container.pageSize];
    const ElementStatus sizeCheck = elementSizeCheck(
        buffer, global->container.pageSize, elementStart, elementLength);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    memcpy(buffer, elementStart, elementLength);
    buffer[elementLength] = '\0';

    if (isPaired) {
        return findOrCreateElement(&global->container, buffer,
                                   &(global->pairedLen), 0, elementID);
    }
    return findOrCreateElement(&global->container, buffer, &(global->singleLen),
                               SINGLE_OFFSET, elementID);
}

unsigned char isSingle(const element_id index) {
    return (index >> SINGLE_MASK) != 0;
}

void printElements(const size_t currentLen,
                   char *const elements[TOTAL_ELEMENTS],
                   const size_t offsetMask) {
    printf("capacity: %zu/%u\n", currentLen, TOTAL_ELEMENTS);
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
    printElements(global->singleLen, global->container.elements, SINGLE_OFFSET);

    printf("paired elements...\n");
    printElements(global->pairedLen, global->container.elements, 0);

    printElementPages(&global->container);
}
void printGlobalTagStatus() {
    printf("printing global tag status...\n\n");
    printCombinedElementStatus(&gTags);
}

void printGlobalAttributeStatus() {
    printf("printing global property status...\n\n");
    printCombinedElementStatus(&gPropKeys);
    printElementStatus(&gPropValues);
}

void printGlobalTextStatus() {
    printf("printing global text status...\n\n");
    printElementStatus(&gText);
}
