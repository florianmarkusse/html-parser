#include <string.h>

#include "flo/html-parser/hash/hashes.h"
#include "flo/html-parser/hash/uint16-t-hash.h"
#include "flo/html-parser/utils/memory/memory.h"

#define MAX_CAPACITY ((1U << 16U) - 1) // Maximum capacity for uint16_t

HashStatus initUint16HashSet(Uint16HashSet *set, const uint16_t capacity) {
    set->arrayLen = capacity;
    set->entries = 0;
    set->array = calloc(capacity, sizeof(Uint16Entry));
    if (set->array == NULL) {
        PRINT_ERROR("Could not allocate memory for element hash set!\n");
        return HASH_ERROR_MEMORY;
    }
    return HASH_SUCCESS;
}

HashStatus insertUint16HashSet(Uint16HashSet *set, const uint16_t id) {
    uint16_t hash = hash16_xm3(id); // Calculate the hash once

    size_t index = hash % set->arrayLen;

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
            PRINT_ERROR("Hash set capacity would exceed the maximum capacity "
                        "for uint16_t!\n");
            return HASH_ERROR_CAPACITY;
        }

        size_t newCapacity = (set->arrayLen * 2 <= MAX_CAPACITY)
                                 ? set->arrayLen * 2
                                 : MAX_CAPACITY;
        Uint16Entry *newArray = calloc(newCapacity, sizeof(Uint16Entry));
        if (newArray == NULL) {
            PRINT_ERROR(
                "Could not allocate memory for element hash set expansion!\n");
            return HASH_ERROR_MEMORY;
        }

        for (size_t i = 0; i < set->arrayLen; i++) {
            if (set->array[i].value != 0) {
                size_t newIndex = set->array[i].hash % newCapacity;
                while (newArray[newIndex].value != 0) {
                    newIndex = (newIndex + 1) % newCapacity;
                }
                newArray[newIndex].value = set->array[i].value;
                newArray[newIndex].hash = set->array[i].hash;
            }
        }

        FREE_TO_NULL(set->array);
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

HashStatus uint16HashSetToArray(const Uint16HashSet *set, uint16_t **results,
                                size_t *resultsLen) {
    *resultsLen = set->entries;
    *results = (uint16_t *)malloc(*resultsLen * sizeof(uint16_t));
    if (*results == NULL) {
        PRINT_ERROR("Could not allocate memory for the result array!\n");
        return HASH_ERROR_MEMORY;
    }

    size_t resultIndex = 0;
    for (size_t i = 0; i < set->arrayLen; i++) {
        if (set->array[i].value != 0) {
            (*results)[resultIndex++] = set->array[i].value;
        }
    }

    return HASH_SUCCESS;
}

bool containsUint16HashSet(const Uint16HashSet *set, const uint16_t id) {
    size_t index = hash16_xm3(id) % set->arrayLen;

    while (set->array[index].value != 0) {
        if (set->array[index].value == id) {
            return true;
        }
        index = (index + 1) % set->arrayLen;
    }

    return false;
}

void destroyUint16HashSet(Uint16HashSet *set) {
    FREE_TO_NULL(set->array);
    set->arrayLen = 0;
    set->entries = 0;
}

HashStatus copyUint16HashSet(const Uint16HashSet *originalSet,
                             Uint16HashSet *copy) {
    HashStatus status = HASH_SUCCESS;
    if ((status = initUint16HashSet(copy, originalSet->arrayLen)) !=
        HASH_SUCCESS) {
        return status;
    }

    size_t arraySize = originalSet->arrayLen * sizeof(Uint16Entry);
    memcpy(copy->array, originalSet->array, arraySize);

    copy->entries = originalSet->entries;

    return status;
}

void resetUint16HashSet(Uint16HashSet *set) {
    if (set->array != NULL) {
        memset(set->array, 0, set->arrayLen * sizeof(Uint16Entry));
    }
    set->entries = 0;
}

void initUint16HashSetIterator(Uint16HashSetIterator *iterator,
                               const Uint16HashSet *set) {
    iterator->set = set;
    iterator->index = 0;
}

uint16_t nextUint16HashSetIterator(Uint16HashSetIterator *iterator) {
    const Uint16HashSet *set = iterator->set;

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

bool hasNextUint16HashSetIterator(Uint16HashSetIterator *iterator) {
    const Uint16HashSet *set = iterator->set;
    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].value != 0) {
            return true;
        }
        iterator->index++;
    }
    return false;
}

void resetUint16HashSetIterator(Uint16HashSetIterator *iterator) {
    iterator->index = 0;
}
