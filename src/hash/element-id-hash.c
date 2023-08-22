

#include "flo/html-parser/hash/element-id-hash.h"
#include "flo/html-parser/hash/hashes.h"
#include "flo/html-parser/utils/memory/memory.h"

HashStatus initElementHashSet(ElementHashSet *set, const size_t capacity) {
    set->arrayLen = capacity;
    set->entries = 0;
    set->array = calloc(capacity, sizeof(element_id));
    if (set->array == NULL) {
        PRINT_ERROR("Could not allocate memory for element hash set!\n");
        return HASH_ERROR_MEMORY;
    }
    return HASH_SUCCESS;
}

HashStatus insertElementHashSet(ElementHashSet *set, const element_id id) {
    if (set->entries >= set->arrayLen) {
        PRINT_ERROR("Element hash set is at full capacity!\n");
        PRINT_ERROR("Could not insert %u!\n", id);
        return HASH_FULL_CAPACITY;
    }

    size_t index = hash16_xm3(id) % set->arrayLen;

    while (set->array[index] != 0) {
        if (set->array[index] == id) {
            return HASH_SUCCESS;
        }
        index = (index + 1) % set->arrayLen;
    }

    set->array[index] = id;
    set->entries++;

    return HASH_SUCCESS;
}

bool containsElementHashSet(const ElementHashSet *set, const element_id id) {
    size_t index = hash16_xm3(id) % set->arrayLen;

    while (set->array[index] != 0) {
        if (set->array[index] == id) {
            return true;
        }
        index = (index + 1) % set->arrayLen;
    }

    return false;
}

void destroyElementHashSet(ElementHashSet *set) {
    FREE_TO_NULL(set->array);
    set->arrayLen = 0;
    set->entries = 0;
}

void initElementHashSetIterator(ElementHashSetIterator *iterator,
                                const ElementHashSet *set) {
    iterator->set = set;
    iterator->index = 0;
}

element_id nextElementHashSetIterator(ElementHashSetIterator *iterator) {
    const ElementHashSet *set = iterator->set;

    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index] != 0) {
            return set->array[iterator->index++];
        }
        iterator->index++;
    }

    return 0;
}

bool hasNextElementHashSetIterator(ElementHashSetIterator *iterator) {
    const ElementHashSet *set = iterator->set;
    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index] != 0) {
            return true;
        }
        iterator->index++;
    }
    return false;
}

void resetElementHashSetIterator(ElementHashSetIterator *iterator) {
    iterator->index = 0;
}
