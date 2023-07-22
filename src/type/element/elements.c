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

ElementStatus createElement(ElementsContainer *container,
                            const char *elementName,
                            element_id *currentElementLen,
                            const element_id offset, element_id *elementID) {
    char *address = NULL;
    DataPageStatus dataPageStatus = insertIntoPage(
        elementName, strlen(elementName) + 1, container->pages, TOTAL_PAGES,
        &container->pageLen, container->pageSize, (void *)&address);
    if (dataPageStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(dataPageStatus),
                               "Could not find or create element \"%s\"",
                               elementName);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    container->elements[offset + *currentElementLen] = address;
    *elementID = offset + (*currentElementLen);
    (*currentElementLen)++;

    return ELEMENT_SUCCESS;
}

ElementStatus findOrCreateElement(ElementsContainer *container,
                                  const char *elementName,
                                  element_id *currentElementLen,
                                  const element_id offset,
                                  element_id *elementID) {
    for (element_id i = offset; i < offset + *currentElementLen; ++i) {
        if (strcmp(container->elements[i], elementName) == 0) {
            *elementID = i;
            return ELEMENT_SUCCESS;
        }
    }

    return createElement(container, elementName, currentElementLen, offset,
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
                             size_t elementLength, element_id *elementID) {
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
                                     size_t elementLength,
                                     unsigned char isPaired,
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

unsigned char isSingle(element_id index) {
    return (index >> TOTAL_ELEMENTS_MSB) != 0;
}

void printCombinedElementStatus(CombinedElements *global) {
    printf("paired elements...\n");
    printf("capacity: %hu/%u\n", global->pairedLen, TOTAL_ELEMENTS / 2);
    for (size_t i = 0; i < global->pairedLen; i++) {
        printf("element ID: %-5zuelement: %-20s\n", i,
               global->container.elements[i]);
    }
    printf("\n");

    printf("single elements...\n");
    printf("capacity: %hu/%u\n", global->singleLen, TOTAL_ELEMENTS / 2);
    for (size_t i = SINGLE_OFFSET; i < SINGLE_OFFSET + global->singleLen; i++) {
        printf("element ID: %-5zuelement: %-20s\n", i,
               global->container.elements[i]);
    }
    printf("\n");

    printf("element pages...\n");
    printf("%-15s: %hhu\n", "pages length", global->container.pageLen);
    for (size_t i = 0; i < global->container.pageLen; i++) {
        printf("%-15s: %lu\n", "space left",
               global->container.pages[i].spaceLeft);

        int printedChars = 0;
        char *copy = global->container.pages[i].start;
        while (printedChars < global->container.pageSize) {
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
void printGlobalTagStatus() {
    printf("printing global tag status...\n\n");
    printCombinedElementStatus(&gTags);
}

void printGlobalAttributeStatus() {
    printf("printing global property status...\n\n");
    printCombinedElementStatus(&gPropKeys);
}

void printGlobalTextStatus() {
    // TODO(florian): print global text but also PROP VALUES I FORGOT
    printf("printing global text status...\n\n");
}
