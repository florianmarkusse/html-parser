#include <string.h>

#include "flo/html-parser/hash/hashes.h"
#include "flo/html-parser/hash/string-hash.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/utils/memory/memory.h"

HashStatus initStringHashSet(StringHashSet *set, const size_t capacity) {
    set->arrayLen = capacity;
    set->entries = 0;
    set->array = calloc(capacity, sizeof(char *));
    if (set->array == NULL) {
        PRINT_ERROR("Could not allocate memory for string hash set!\n");
        return HASH_ERROR_MEMORY;
    }
    return HASH_SUCCESS;
}

HashStatus insertStringHashSet(StringHashSet *set, const char *string) {
    if (set->entries >= set->arrayLen) {
        PRINT_ERROR("String hash set is at full capacity!\n");
        PRINT_ERROR("Could not insert %s!\n", string);
        return HASH_FULL_CAPACITY;
    }

    size_t hash = hashString(string) % set->arrayLen;

    while (set->array[hash] != NULL) {
        if (strcmp(set->array[hash], string) == 0) {
            return HASH_SUCCESS;
        }
        hash = (hash + 1) % set->arrayLen;
    }

    set->array[hash] = string;
    set->entries++;

    return HASH_SUCCESS;
}

HashStatus insertWithHashStringHashSet(StringHashSet *set, size_t hash) {}

bool containsStringHashSet(const StringHashSet *set, const char *string) {
    size_t index = hashString(string) % set->arrayLen;

    while (set->array[index] != NULL) {
        if (strcmp(set->array[index], string) == 0) {
            return true;
        }
        index = (index + 1) % set->arrayLen;
    }

    return false;
}

void destroyStringHashSet(StringHashSet *set) {
    FREE_TO_NULL(set->array);
    set->arrayLen = 0;
    set->entries = 0;
}

void initStringHashSetIterator(StringHashSetIterator *iterator,
                               const StringHashSet *set) {
    iterator->set = set;
    iterator->index = 0;
}

const char *nextStringHashSetIterator(StringHashSetIterator *iterator) {
    const StringHashSet *set = iterator->set;

    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index] != NULL) {
            return set->array[iterator->index++];
        }
        iterator->index++;
    }

    return 0;
}

bool hasNextStringHashSetIterator(StringHashSetIterator *iterator) {
    const StringHashSet *set = iterator->set;
    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index] != 0) {
            return true;
        }
        iterator->index++;
    }
    return false;
}

void stringHashSetIteratorReset(StringHashSetIterator *iterator) {
    iterator->index = 0;
}
