#include <stdint.h>
#include <stdio.h>

#include "flo/html-parser/type/element/elements-print.h"

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
