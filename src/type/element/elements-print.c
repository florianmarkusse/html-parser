#include <stdio.h>

#include "flo/html-parser/type/element/elements-print.h"

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

void printStringRegistryStatus(const StringRegistry *newElements) {
    printf("hash set contents...\n");
    StringHashSetIterator iterator;
    initStringHashSetIterator(&iterator, &newElements->set);

    while (hasNextStringHashSetIterator(&iterator)) {
        const char *string = nextStringHashSetIterator(&iterator);
        printf("%s\n", string);
    }

    printElementPages(&newElements->container);
}

void printPropertyStatus(DataContainer *dataContainer) {
    printf("printing property status...\n\n");
    printf("printing keys...\n");
    printStringRegistryStatus(&dataContainer->propKeys);
    printf("printing values...\n");
    printStringRegistryStatus(&dataContainer->propValues);
}

void printBoolPropStatus(DataContainer *dataContainer) {
    printf("printing bool property status...\n\n");
    printStringRegistryStatus(&dataContainer->boolProps);
}

void printTextStatus(DataContainer *dataContainer) {
    printf("printing text status...\n\n");
    printStringRegistryStatus(&dataContainer->text);
}

void printTagStatus(DataContainer *dataContainer) {
    printf("printing tags status...\n\n");
    printStringRegistryStatus(&dataContainer->tags);
}
