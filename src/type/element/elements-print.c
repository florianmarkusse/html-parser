#include <stdint.h>
#include <stdio.h>

#include "flo/html-parser/type/element/elements-print.h"

void printElements(const ptrdiff_t currentLen,
                   char *const elements[FLO_HTML_TOTAL_ELEMENTS],
                   const ptrdiff_t offsetMask) {
    printf("capacity: %zu/%u\n", currentLen, FLO_HTML_POSSIBLE_ELEMENTS);
    for (ptrdiff_t i = offsetMask; i < (offsetMask | currentLen); i++) {
        printf("element ID: %-7zuelement: %-20s\n", i, elements[i]);
    }
    printf("\n\n");
}

void printPage(const flo_html_DataPage *page) {
    printf("data page...\n");

    uintptr_t spaceUsed = page->freeSpace - page->start;

    for (ptrdiff_t i = 0; i < spaceUsed; i++) {
        printf("%c", ((unsigned char *)page->start)[i]);
    }
    printf("\n");
    printf("%-15s: %lu\n", "space left", page->spaceLeft);

    printf("\n\n");
}

void printflo_html_StringRegistryStatus(
    const flo_html_StringRegistry *newElements) {
    printf("hash set contents...\n");
    flo_html_StringHashSetIterator iterator;
    flo_html_initStringHashSetIterator(&iterator, &newElements->set);

    while (flo_html_hasNextStringHashSetIterator(&iterator)) {
        const flo_html_String string =
            flo_html_nextStringHashSetIterator(&iterator);
        printf("%.*s\n", FLO_HTML_S_P(string));
    }

    printPage(&newElements->dataPage);
}

void flo_html_printPropertyStatus(const flo_html_TextStore *textStore) {
    printf("printing property status...\n\n");
    printf("printing keys...\n");
    printflo_html_StringRegistryStatus(&textStore->propKeys);
    printf("printing values...\n");
    printflo_html_StringRegistryStatus(&textStore->propValues);
}

void flo_html_printBoolPropStatus(const flo_html_TextStore *textStore) {
    printf("printing bool property status...\n\n");
    printflo_html_StringRegistryStatus(&textStore->boolProps);
}

void flo_html_printTagStatus(const flo_html_TextStore *textStore) {
    printf("printing tags status...\n\n");
    printflo_html_StringRegistryStatus(&textStore->tags);
}
