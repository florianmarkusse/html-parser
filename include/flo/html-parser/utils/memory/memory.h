#ifndef FLO_HTML_PARSER_UTILS_MEMORY_MEMORY_H
#define FLO_HTML_PARSER_UTILS_MEMORY_MEMORY_H

#include <stdlib.h>

#include "flo/html-parser/utils/print/error.h"

/**
 * @brief Free a pointer and set it to NULL.
 *
 * This macro frees the memory pointed to by the given pointer and sets the
 * pointer itself to NULL. It is a safe way to free memory and avoid
 * dereferencing a dangling pointer.
 *
 * @param[in,out] ptr   A pointer to be freed and set to NULL.
 */
#define FLO_HTML_FREE_TO_NULL(ptr)                                                      \
    do {                                                                       \
        free(ptr);                                                             \
        (ptr) = NULL;                                                          \
    } while (0)

/**
 * @brief Resize a dynamically allocated array.
 *
 * This inline function resizes a dynamically allocated array when the current
 * length exceeds the current capacity. It ensures that enough memory is
 * allocated for the array to hold additional elements. If the reallocation
 * fails, it prints an error message.
 *
 * @param[in]       array           The current array to be resized.
 * @param[in]       currentLen      The current length of the array.
 * @param[in,out]   currentCap      The current capacity of the array (updated
 *                                  if reallocation occurs).
 * @param[in]       elementSize     The size of each element in the array.
 * @param[in]       extraElements   The number of extra elements to accommodate.
 *
 * @return  A pointer to the resized array, or NULL if reallocation fails.
 */
static inline void *flo_html_resizeArray(void *array, size_t currentLen,
                                size_t *currentCap, size_t elementSize,
                                size_t extraElements) {
    if (currentLen < *currentCap) {
        return array;
    }
    size_t newCap = (currentLen + extraElements);
    void *newArray = realloc(array, newCap * elementSize);
    if (newArray == NULL) {
        FLO_HTML_PRINT_ERROR("Failed to reallocate more memory for the array.\n");
        return NULL;
    }
    *currentCap = newCap;
    return newArray;
}

#endif
