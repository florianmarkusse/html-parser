#include <string.h>

#include "flo/html-parser/hash/hashes.h"
#include "flo/html-parser/hash/uint16-t-hash.h"
#include "flo/html-parser/util//memory.h"

#define MAX_CAPACITY ((1U << 16U) - 1) // Maximum capacity for uint16_t

flo_html_HashStatus flo_html_initUint16HashSet(flo_html_Uint16HashSet *set,
                                               const uint16_t capacity) {
    set->arrayLen = capacity;
    set->entries = 0;
    set->array = calloc(capacity, sizeof(flo_html_Uint16Entry));
    if (set->array == NULL) {
        FLO_HTML_PRINT_ERROR(
            "Could not allocate memory for element hash set!\n");
        return HASH_ERROR_MEMORY;
    }
    return HASH_SUCCESS;
}

flo_html_HashStatus flo_html_insertUint16HashSet(flo_html_Uint16HashSet *set,
                                                 const uint16_t id) {
    uint16_t hash = flo_html_hash16_xm3(id); // Calculate the hash once

    ptrdiff_t index = hash % set->arrayLen;

    while (set->array[index].value != 0) {
        if (set->array[index].value == id) {
            return HASH_SUCCESS; // Element already in the set, nothing to do
        }
        index = (index + 1) % set->arrayLen;
    }

    bool didResize = false;
    if (set->entries >= set->arrayLen * 0.7) {
        didResize = true;
        // See if it makes sense to grow.
        if (set->arrayLen >= MAX_CAPACITY * 0.9) {
            FLO_HTML_PRINT_ERROR(
                "Hash set capacity would exceed the maximum capacity "
                "for uint16_t!\n");
            return HASH_ERROR_CAPACITY;
        }

        ptrdiff_t newCapacity = (set->arrayLen * 2 <= MAX_CAPACITY)
                                    ? set->arrayLen * 2
                                    : MAX_CAPACITY;
        flo_html_Uint16Entry *newArray =
            calloc(newCapacity, sizeof(flo_html_Uint16Entry));
        if (newArray == NULL) {
            FLO_HTML_PRINT_ERROR(
                "Could not allocate memory for element hash set expansion!\n");
            return HASH_ERROR_MEMORY;
        }

        for (ptrdiff_t i = 0; i < set->arrayLen; i++) {
            if (set->array[i].value != 0) {
                ptrdiff_t newIndex = set->array[i].hash % newCapacity;
                while (newArray[newIndex].value != 0) {
                    newIndex = (newIndex + 1) % newCapacity;
                }
                newArray[newIndex].value = set->array[i].value;
                newArray[newIndex].hash = set->array[i].hash;
            }
        }

        FLO_HTML_FREE_TO_NULL(set->array);
        set->array = newArray;
        set->arrayLen = newCapacity;
    }

    if (didResize) {
        index = hash % set->arrayLen;
        while (set->array[index].value != 0) {
            if (set->array[index].value == id) {
                break;
            }
            index = (index + 1) % set->arrayLen;
        }
    }

    set->array[index].value = id;
    set->array[index].hash = hash;
    set->entries++;

    return HASH_SUCCESS;
}

flo_html_HashStatus
flo_html_uint16HashSetToArray(const flo_html_Uint16HashSet *set,
                              uint16_t **results, ptrdiff_t *resultsLen) {
    *resultsLen = set->entries;
    *results = (uint16_t *)malloc(*resultsLen * sizeof(uint16_t));
    if (*results == NULL) {
        FLO_HTML_PRINT_ERROR(
            "Could not allocate memory for the result array!\n");
        return HASH_ERROR_MEMORY;
    }

    ptrdiff_t resultIndex = 0;
    for (ptrdiff_t i = 0; i < set->arrayLen; i++) {
        if (set->array[i].value != 0) {
            (*results)[resultIndex++] = set->array[i].value;
        }
    }

    return HASH_SUCCESS;
}

bool flo_html_containsUint16HashSet(const flo_html_Uint16HashSet *set,
                                    const uint16_t id) {
    ptrdiff_t index = flo_html_hash16_xm3(id) % set->arrayLen;

    while (set->array[index].value != 0) {
        if (set->array[index].value == id) {
            return true;
        }
        index = (index + 1) % set->arrayLen;
    }

    return false;
}

void flo_html_destroyUint16HashSet(flo_html_Uint16HashSet *set) {
    FLO_HTML_FREE_TO_NULL(set->array);
    set->arrayLen = 0;
    set->entries = 0;
}

flo_html_HashStatus
flo_html_copyUint16HashSet(const flo_html_Uint16HashSet *originalSet,
                           flo_html_Uint16HashSet *copy) {
    flo_html_HashStatus status = HASH_SUCCESS;
    if ((status = flo_html_initUint16HashSet(copy, originalSet->arrayLen)) !=
        HASH_SUCCESS) {
        return status;
    }

    ptrdiff_t arraySize = originalSet->arrayLen * sizeof(flo_html_Uint16Entry);
    memcpy(copy->array, originalSet->array, arraySize);

    copy->entries = originalSet->entries;

    return status;
}

void flo_html_resetUint16HashSet(flo_html_Uint16HashSet *set) {
    if (set->array != NULL) {
        memset(set->array, 0, set->arrayLen * sizeof(flo_html_Uint16Entry));
    }
    set->entries = 0;
}

void flo_html_initUint16HashSetIterator(
    flo_html_Uint16HashSetIterator *iterator,
    const flo_html_Uint16HashSet *set) {
    iterator->set = set;
    iterator->index = 0;
}

uint16_t
flo_html_nextUint16HashSetIterator(flo_html_Uint16HashSetIterator *iterator) {
    const flo_html_Uint16HashSet *set = iterator->set;

    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].value != 0) {
            const uint16_t value = set->array[iterator->index].value;
            iterator->index++;
            return value;
        }
        iterator->index++;
    }

    return 0;
}

bool flo_html_hasNextUint16HashSetIterator(
    flo_html_Uint16HashSetIterator *iterator) {
    const flo_html_Uint16HashSet *set = iterator->set;
    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].value != 0) {
            return true;
        }
        iterator->index++;
    }
    return false;
}

void flo_html_resetUint16HashSetIterator(
    flo_html_Uint16HashSetIterator *iterator) {
    iterator->index = 0;
}
