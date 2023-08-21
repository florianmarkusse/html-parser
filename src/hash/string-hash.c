#include <string.h>

#include "flo/html-parser/hash/string-hash.h"

// http://www.cse.yorku.ca/~oz/hash.html
// djb2 hash
size_t hashString(const char *string) {
    size_t hash = 5381;
    int c = 0;

    while (c = *string++) {
        hash =
            ((hash << 5) + hash) ^
            c; // hash << 5 + hash because that is way cooler than hash * 33 :D
    }

    return hash;
}

HashStatus stringHashSetInit(StringHashSet *set, const size_t capacity) {
    set->arrayLen = capacity;
    set->entries = 0;
    set->array = calloc(capacity, sizeof(char *));
    if (set->array == NULL) {
        return HASH_ERROR_MEMORY;
    }
    return HASH_SUCCESS;
}

HashStatus stringHashSetInsert(StringHashSet *set, const char *string) {
    if (set->entries >= set->arrayLen) {
        return HASH_FULL_CAPACITY;
    }

    size_t index = hashString(string) % set->arrayLen;

    while (set->array[index] != NULL) {
        if (strcmp(set->array[index], string) == 0) {
            return HASH_SUCCESS;
        }
        index = (index + 1) % set->arrayLen;
    }

    set->array[index] = string;
    set->entries++;

    return HASH_SUCCESS;
}

bool stringHashSetContains(const StringHashSet *set, const char *string) {
    size_t index = hashString(string) % set->arrayLen;

    while (set->array[index] != NULL) {
        if (strcmp(set->array[index], string) == 0) {
            return true;
        }
        index = (index + 1) % set->arrayLen;
    }

    return false;
}

void stringHashSetDestroy(StringHashSet *set) {
    free(set->array);
    set->arrayLen = 0;
    set->entries = 0;
}

void stringHashSetIteratorInit(StringHashSetIterator *iterator,
                               const StringHashSet *set) {
    iterator->set = set;
    iterator->index = 0;
}

const char *stringHashSetIteratorNext(StringHashSetIterator *iterator) {
    const StringHashSet *set = iterator->set;

    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index] != NULL) {
            return set->array[iterator->index++];
        }
        iterator->index++;
    }

    return 0;
}

bool stringHashSetIteratorHasNext(StringHashSetIterator *iterator) {
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
