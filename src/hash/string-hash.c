#include <stdio.h>
#include <string.h>

#include "flo/html-parser/hash/hashes.h"
#include "flo/html-parser/hash/string-hash.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/utils/memory/memory.h"

#define MAX_PROBES (1U << 4U)

flo_html_HashStatus flo_html_initStringHashSet(flo_html_StringHashSet *set,
                                               const size_t capacity) {
    set->arrayLen = capacity;
    set->entries = 0;
    set->array = calloc(capacity, sizeof(flo_html_HashEntry));
    if (set->array == NULL) {
        FLO_HTML_PRINT_ERROR(
            "Could not allocate memory for string hash set!\n");
        return HASH_ERROR_MEMORY;
    }
    return HASH_SUCCESS;
}

// Sets the flo_html_indexID that is used in the DOM, starting at 1 because then
// 0 can be used as an error/init value.
flo_html_HashStatus
flo_html_insertStringAtHash(flo_html_StringHashSet *set,
                            const flo_html_String string,
                            const flo_html_HashElement *hashElement,
                            flo_html_indexID *flo_html_indexID) {
    if (set->entries >= set->arrayLen) {
        FLO_HTML_PRINT_ERROR("String hash set is at full capacity!\n");
        return HASH_ERROR_CAPACITY;
    }

    set->entries++;

    const size_t arrayIndex =
        (hashElement->hash + hashElement->offset) % set->arrayLen;
    set->array[arrayIndex].string = string;
    set->array[arrayIndex].flo_html_indexID = set->entries;
    *flo_html_indexID = set->entries;

    return HASH_SUCCESS;
}

// Sets the flo_html_indexID that is used in the DOM, starting at 1 because then
// 0 can be used as an error/init value.
flo_html_HashStatus flo_html_insertStringHashSet(flo_html_StringHashSet *set,
                                                 const flo_html_String string) {
    if (set->entries >= set->arrayLen) {
        FLO_HTML_PRINT_ERROR("String hash set is at full capacity!\n");
        FLO_HTML_PRINT_ERROR("Could not insert %s!\n", string.buf);
        return HASH_ERROR_CAPACITY;
    }

    size_t hash = flo_html_hashString(string) % set->arrayLen;

    while (set->array[hash].string.buf != NULL) {
        if (flo_html_stringEquals(set->array[hash].string, string)) {
            return HASH_SUCCESS;
        }
        hash = (hash + 1) % set->arrayLen;
    }

    set->entries++;

    set->array[hash].string = string;
    set->array[hash].flo_html_indexID = set->entries;

    return HASH_SUCCESS;
}

bool flo_html_containsStringHashSet(const flo_html_StringHashSet *set,
                                    const flo_html_String string) {
    flo_html_HashElement ignore;
    flo_html_indexID ignore2 = 0;
    return flo_html_containsStringWithDataHashSet(set, string, &ignore,
                                                  &ignore2);
}

bool flo_html_containsStringWithDataHashSet(
    const flo_html_StringHashSet *set, const flo_html_String string,
    flo_html_HashElement *hashElement, flo_html_indexID *flo_html_indexID) {
    size_t index = flo_html_hashString(string) % set->arrayLen;
    hashElement->hash = index;

    size_t probes = 0;
    while (set->array[index].string.buf != NULL) {
        flo_html_HashEntry entry = set->array[index];
        if (flo_html_stringEquals(entry.string, string)) {
            hashElement->offset = probes;
            *flo_html_indexID = entry.flo_html_indexID;
            return true;
        }
        probes++;
        index = (index + 1) % set->arrayLen;
    }

    hashElement->offset = probes;
    return false;
}

const flo_html_String
flo_html_getStringFromHashSet(const flo_html_StringHashSet *set,
                              const flo_html_HashElement *hashElement) {
    return set
        ->array[((hashElement->hash + hashElement->offset) % set->arrayLen)]
        .string;
}

void flo_html_destroyStringHashSet(flo_html_StringHashSet *set) {
    FLO_HTML_FREE_TO_NULL(set->array);
    set->arrayLen = 0;
    set->entries = 0;
}

flo_html_ComparisonStatus
flo_html_equalsStringHashSet(const flo_html_StringHashSet *set1,
                             const flo_html_StringHashSet *set2) {
    if (set1->entries != set2->entries) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    flo_html_StringHashSetIterator iterator;
    flo_html_initStringHashSetIterator(&iterator, set1);

    while (flo_html_hasNextStringHashSetIterator(&iterator)) {
        const flo_html_String element =
            flo_html_nextStringHashSetIterator(&iterator);
        if (!flo_html_containsStringHashSet(set2, element)) {
            return COMPARISON_DIFFERENT_CONTENT;
        }
    }

    flo_html_initStringHashSetIterator(&iterator, set2);

    while (flo_html_hasNextStringHashSetIterator(&iterator)) {
        const flo_html_String element =
            flo_html_nextStringHashSetIterator(&iterator);
        if (!flo_html_containsStringHashSet(set1, element)) {
            return COMPARISON_DIFFERENT_CONTENT;
        }
    }

    return COMPARISON_SUCCESS;
}

void flo_html_initStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator,
    const flo_html_StringHashSet *set) {
    iterator->set = set;
    iterator->index = 0;
}

const flo_html_String
flo_html_nextStringHashSetIterator(flo_html_StringHashSetIterator *iterator) {
    const flo_html_StringHashSet *set = iterator->set;

    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].flo_html_indexID > 0) {
            printf("Returining a string...");
            printf("index is %zu\n", iterator->index);
            flo_html_String string = set->array[iterator->index].string;
            printf("string is %.*s\n", FLO_HTML_S_P(string));
            return set->array[iterator->index++].string;
        }
        iterator->index++;
    }

    return FLO_HTML_EMPTY_STRING;
}

bool flo_html_hasNextStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator) {
    const flo_html_StringHashSet *set = iterator->set;
    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index].flo_html_indexID > 0) {
            return true;
        }
        iterator->index++;
    }
    return false;
}

void flo_html_resetStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator) {
    iterator->index = 0;
}
