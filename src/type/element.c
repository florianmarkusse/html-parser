#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type/data-page.h"
#include "type/element.h"
#include "utils/print/error.h"

Elements globalTags;
Elements globalAttributes;

ElementStatus createGlobals() {
    globalTags.pairedLen = 0;
    globalTags.singleLen = 0;
    globalTags.pageLen = 0;
    globalAttributes.pairedLen = 0;
    globalAttributes.singleLen = 0;
    globalAttributes.pageLen = 0;
    return ELEMENT_SUCCESS;
}

void destroyGlobals() {
    for (int i = 0; i < globalTags.pageLen; i++) {
        free(globalTags.pages[i].start);
    }
    globalTags.pageLen = 0;
    globalTags.pairedLen = 0;
    globalTags.singleLen = 0;

    for (int i = 0; i < globalAttributes.pageLen; i++) {
        free(globalAttributes.pages[i].start);
    }
    globalAttributes.pageLen = 0;
    globalAttributes.pairedLen = 0;
    globalAttributes.singleLen = 0;
}

ElementStatus findOrCreateElement(Elements *global, const char *elementName,
                                  element_id *currentElementLen,
                                  element_id offset, element_id *elementID) {
    for (element_id i = offset; i < offset + *currentElementLen; ++i) {
        // Check if element already exists
        if (strcmp(global->elements[i], elementName) == 0) {
            *elementID = i;
            return ELEMENT_SUCCESS;
        }
    }

    char *address = NULL;
    DataPageStatus dataPageStatus =
        insertIntoPage(elementName, strlen(elementName) + 1, global->pages,
                       TOTAL_PAGES, &global->pageLen, (void *)&address);
    if (dataPageStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(dataPageStatus),
                               "Could not find or create element \"%s\"",
                               elementName);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    global->elements[offset + *currentElementLen] = address;
    *elementID = offset + (*currentElementLen);
    (*currentElementLen)++;

    return ELEMENT_SUCCESS;
}

ElementStatus elementToIndex(Elements *global, const char *elementStart,
                             size_t elementLength, unsigned char isPaired,
                             element_id *elementID) {
    char buffer[PAGE_SIZE];

    if (elementLength >= PAGE_SIZE) {
        PRINT_ERROR("Tag is too long to fit into page.\n");
        PRINT_ERROR("Printing first part of tag:\n");

        memcpy(buffer, elementStart, PAGE_SIZE - 1);
        buffer[PAGE_SIZE - 1] = '\0';

        PRINT_ERROR("%s\n", buffer);
        PRINT_ERROR("max size is %u\n", PAGE_SIZE);

        return ELEMENT_TOO_LONG;
    }

    memcpy(buffer, elementStart, elementLength);
    buffer[elementLength] = '\0';

    if (isPaired) {
        return findOrCreateElement(global, buffer, &(global->pairedLen), 0,
                                   elementID);
    }
    return findOrCreateElement(global, buffer, &(global->singleLen),
                               SINGLE_OFFSET, elementID);
}

unsigned char isSingle(element_id index) {
    return (index >> TOTAL_ELEMENTS_MSB) != 0;
}

void printElementStatus(Elements *global) {
    printf("paired elements...\n");
    printf("capacity: %hu/%u\n", global->pairedLen, TOTAL_ELEMENTS / 2);
    for (size_t i = 0; i < global->pairedLen; i++) {
        printf("element ID: %-5zuelement: %-20s\n", i, global->elements[i]);
    }
    printf("\n");

    printf("single elements...\n");
    printf("capacity: %hu/%u\n", global->singleLen, TOTAL_ELEMENTS / 2);
    for (size_t i = SINGLE_OFFSET; i < SINGLE_OFFSET + global->singleLen; i++) {
        printf("element ID: %-5zuelement: %-20s\n", i, global->elements[i]);
    }
    printf("\n");

    printf("element pages...\n");
    printf("%-15s: %hhu\n", "pages length", global->pageLen);
    for (size_t i = 0; i < global->pageLen; i++) {
        printf("%-15s: %hu\n", "space left", global->pages[i].spaceLeft);

        int printedChars = 0;
        char *copy = global->pages[i].start;
        while (printedChars < PAGE_SIZE) {
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
    printElementStatus(&globalTags);
}

void printGlobalAttributeStatus() {
    printf("printing global attribute status...\n\n");
    printElementStatus(&globalAttributes);
}
