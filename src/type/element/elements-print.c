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

void printNewElementsStatus(const NewElements *newElements) {
    printf("hash set contents...\n");
    StringHashSetIterator iterator;
    initStringHashSetIterator(&iterator, &newElements->set);

    while (hasNextStringHashSetIterator(&iterator)) {
        const char *string = nextStringHashSetIterator(&iterator);
        printf("%s\n", string);
    }

    printElementPages(&newElements->container);
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

void printTagNamesStatus(DataContainer *dataContainer) {
    printf("printing tagNames status...\n\n");
    printNewElementsStatus(&dataContainer->tagNames);
}
