#ifndef UTILS_MEMORY_MEMORY_H
#define UTILS_MEMORY_MEMORY_H

#include <stdlib.h>

#include "utils/print/error.h"

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
