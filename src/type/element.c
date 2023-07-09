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

ElementStatus findOrCreateElement(Elements *global, const char *tagName,
                                  element_id *currentTagLen, element_id offset,
                                  element_id *elementID) {
    for (element_id i = offset; i < offset + *currentTagLen; ++i) {
        // Check if tag already exists
        if (strcmp(global->elements[i], tagName) == 0) {
            *elementID = i;
            return ELEMENT_SUCCESS;
        }
    }

    char *address = NULL;
    DataPageStatus dataPageStatus =
        insertIntoPage(tagName, strlen(tagName) + 1, global->pages, TOTAL_PAGES,
                       &global->pageLen, (void *)&address);
    if (dataPageStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(dataPageStatus),
                               "Could not find or create tag \"%s\"", tagName);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    global->elements[offset + *currentTagLen] = address;
    *elementID = offset + (*currentTagLen);
    (*currentTagLen)++;

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
                               SINGLE_TAGS_OFFSET, elementID);
}

unsigned char isSingle(element_id index) {
    return (index >> TOTAL_ELEMENTS_MSB) != 0;
}

void printElementStatus() {
    printf("printing global tag status...\n\n");

    printf("paired tags...\n");
    printf("capacity: %hu/%u\n", globalTags.pairedLen, TOTAL_ELEMENTS / 2);
    for (size_t i = 0; i < globalTags.pairedLen; i++) {
        printf("tag ID: %-5zutag: %-20s\n", i, globalTags.elements[i]);
    }
    printf("\n");

    printf("single tags...\n");
    printf("capacity: %hu/%u\n", globalTags.singleLen, TOTAL_ELEMENTS / 2);
    for (size_t i = SINGLE_TAGS_OFFSET;
         i < SINGLE_TAGS_OFFSET + globalTags.singleLen; i++) {
        printf("tag ID: %-5zutag: %-20s\n", i, globalTags.elements[i]);
    }
    printf("\n");

    printf("tag pages...\n");
    printf("%-15s: %hhu\n", "pages length", globalTags.pageLen);
    for (size_t i = 0; i < globalTags.pageLen; i++) {
        printf("%-15s: %hu\n", "space left", globalTags.pages[i].spaceLeft);

        int printedChars = 0;
        char *copy = globalTags.pages[i].start;
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
