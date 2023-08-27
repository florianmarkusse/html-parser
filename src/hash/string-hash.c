#include <string.h>

#include "flo/html-parser/hash/hashes.h"
#include "flo/html-parser/hash/string-hash.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/utils/memory/memory.h"

#define MAX_PROBES (1U << 4U)

HashStatus initStringHashSet(StringHashSet *set, const size_t capacity) {
    set->arrayLen = capacity;
    set->entries = 0;
    set->array = calloc(capacity, sizeof(HashEntry));
    if (set->array == NULL) {
        PRINT_ERROR("Could not allocate memory for string hash set!\n");
        return HASH_ERROR_MEMORY;
    }
    return HASH_SUCCESS;
}

// Sets the indexID that is used in the DOM, starting at 1 because then 0 can be
// used as an error/init value.
HashStatus insertStringWithDataHashSet(StringHashSet *set, const char *string,
                                       HashElement *hashElement,
                                       indexID *indexID) {
    if (set->entries >= set->arrayLen) {
        PRINT_ERROR("String hash set is at full capacity!\n");
        return HASH_FULL_CAPACITY;
    }

    size_t hash = hashString(string) % set->arrayLen;
    hashElement->hash = hash;

    size_t probes = 0;
    while (set->array[hash].string != NULL) {
        if (strcmp(set->array[hash].string, string) == 0) {
            break;
        }
        if (probes > MAX_PROBES) {
            PRINT_ERROR("Reached maximum number of probes, %zu!\n", probes);
            return HASH_MAX_PROBES;
        }
        probes++;
        hash = (hash + 1) % set->arrayLen;
    }
    hashElement->offset = probes;

    set->entries++;

    set->array[hash].string = string;
    set->array[hash].indexID = set->entries;
    *indexID = set->entries;

    return HASH_SUCCESS;
}

HashStatus insertStringHashSet(StringHashSet *set, const char *string) {
    HashElement ignore;
    indexID ignore2 = 0;
    return insertStringWithDataHashSet(set, string, &ignore, &ignore2);
}

bool containsStringHashSet(const StringHashSet *set, const char *string) {
    indexID ignore = 0;
    return containsStringWithDataHashSet(set, string, &ignore);
}

bool containsStringWithDataHashSet(const StringHashSet *set, const char *string,
                                   indexID *indexID) {
    size_t index = hashString(string) % set->arrayLen;

    while (set->array[index].string != NULL) {
        HashEntry entry = set->array[index];
        if (strcmp(entry.string, string) == 0) {
            *indexID = entry.indexID;
            return true;
        }
        index = (index + 1) % set->arrayLen;
    }

    return false;
}

const char *getStringFromHashSet(const StringHashSet *set,
                                 const HashElement *hashElement) {
    return set
        ->array[((hashElement->hash + hashElement->offset) % set->arrayLen)]
        .string;
}

void destroyStringHashSet(StringHashSet *set) {
    FREE_TO_NULL(set->array);
    set->arrayLen = 0;
    set->entries = 0;
}

ComparisonStatus equalsStringHashSet(const StringHashSet *set1,
                                     const StringHashSet *set2) {
    if (set1->entries != set2->entries) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    StringHashSetIterator iterator;
    initStringHashSetIterator(&iterator, set1);

    while (hasNextStringHashSetIterator(&iterator)) {
        const char *element = nextStringHashSetIterator(&iterator);
        if (!containsStringHashSet(set2, element)) {
            return COMPARISON_DIFFERENT_CONTENT;
        }
    }

    initStringHashSetIterator(&iterator, set2);

    while (hasNextStringHashSetIterator(&iterator)) {
        const char *element = nextStringHashSetIterator(&iterator);
        if (!containsStringHashSet(set1, element)) {
            return COMPARISON_DIFFERENT_CONTENT;
        }
    }

    return COMPARISON_SUCCESS;
}

void initStringHashSetIterator(StringHashSetIterator *iterator,
                               const StringHashSet *set) {
    iterator->set = set;
    iterator->index = 0;
}

const char *nextStringHashSetIterator(StringHashSetIterator *iterator) {
    const StringHashSet *set = iterator->set;

    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].string != NULL) {
            return set->array[iterator->index++].string;
        }
        iterator->index++;
    }

    return NULL;
}

bool hasNextStringHashSetIterator(StringHashSetIterator *iterator) {
    const StringHashSet *set = iterator->set;
    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].string != NULL) {
            return true;
        }
        iterator->index++;
    }
    return false;
}

void resetStringHashSetIterator(StringHashSetIterator *iterator) {
    iterator->index = 0;
}
