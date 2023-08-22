#ifndef FLO_HTML_PARSER_UTILS_MEMORY_MEMORY_H
#define FLO_HTML_PARSER_UTILS_MEMORY_MEMORY_H

#include <stdlib.h>

#include "flo/html-parser/utils/print/error.h"

#define FREE_TO_NULL(ptr)                                                      \
    do {                                                                       \
        free(ptr);                                                             \
        (ptr) = NULL;                                                          \
    } while (0)

static inline void *resizeArray(void *array, size_t currentLen,
                                size_t *currentCap, size_t elementSize,
                                size_t extraElements) {
    if (currentLen < *currentCap) {
        return array;
    }
    size_t newCap = (currentLen + extraElements);
    void *newArray = realloc(array, newCap * elementSize);
    if (newArray == NULL) {
        PRINT_ERROR("Failed to reallocate more memory for the array.\n");
        return NULL;
    }
    *currentCap = newCap;
    return newArray;
}

#endif
