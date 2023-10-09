#include <stdio.h>
#include <string.h>

#include "flo/html-parser/util/hash/hash-element.h"
#include "flo/html-parser/util/hash/hashes.h"
#include "flo/html-parser/util/hash/string-hash.h"
#include "flo/html-parser/util/memory.h"

#define MAX_PROBES (1U << 4U)

void flo_html_initStringHashSet(flo_html_StringHashSet *set,
                                const ptrdiff_t capacity,
                                flo_html_Arena *perm) {
    flo_html_String *thing = FLO_HTML_NEW(perm, thing);

    set->arrayLen = capacity;
    set->entries = 0;
    set->array = FLO_HTML_NEW(perm, flo_html_StringHashEntry, capacity,
                              FLO_HTML_ZERO_MEMORY);
}

// Sets the flo_html_index_id that is used in the DOM, starting at 1 because
// then 0 can be used as an error/init value.
flo_html_index_id
flo_html_insertStringAtHash(flo_html_StringHashSet *set,
                            const flo_html_String string,
                            const flo_html_HashElement *hashElement) {
    if (set->entries >= set->arrayLen) {
        FLO_HTML_PRINT_ERROR("String hash set is at full capacity!\n");
        return HASH_ERROR_CAPACITY;
    }

    set->entries++;

    const size_t arrayIndex =
        (hashElement->hash + hashElement->offset) % set->arrayLen;
    set->array[arrayIndex].string = string;
    set->array[arrayIndex].indexID = set->entries;
    return set->entries;
}

// Sets the flo_html_index_id that is used in the DOM, starting at 1 because
// then 0 can be used as an error/init value.
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
    set->array[hash].indexID = set->entries;

    return HASH_SUCCESS;
}

bool flo_html_containsStringHashSet(const flo_html_StringHashSet *set,
                                    const flo_html_String string) {
    return flo_html_containsStringWithDataHashSet(set, string).wasPresent;
}

flo_html_Contains
flo_html_containsStringWithDataHashSet(const flo_html_StringHashSet *set,
                                       const flo_html_String string) {
    flo_html_Contains result;

    size_t hash = flo_html_hashString(string);
    result.hashEntry.hashElement.hash = hash;
    ptrdiff_t index = hash % set->arrayLen;

    ptrdiff_t probes = 0;
    while (set->array[index].string.buf != NULL) {
        flo_html_StringHashEntry entry = set->array[index];
        if (flo_html_stringEquals(entry.string, string)) {
            result.hashEntry.hashElement.offset = probes;
            result.hashEntry.entryID = entry.indexID;
            result.wasPresent = true;
            return result;
        }
        probes++;
        index = (index + 1) % set->arrayLen;
    }

    result.hashEntry.hashElement.offset = probes;
    result.wasPresent = false;
    return result;
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

flo_html_HashComparisonStatus
flo_html_equalsStringHashSet(const flo_html_StringHashSet *set1,
                             const flo_html_StringHashSet *set2) {
    if (set1->entries != set2->entries) {
        return HASH_COMPARISON_DIFFERENT_SIZES;
    }

    flo_html_StringHashSetIterator iterator;
    flo_html_initStringHashSetIterator(&iterator, set1);

    while (flo_html_hasNextStringHashSetIterator(&iterator)) {
        const flo_html_String element =
            flo_html_nextStringHashSetIterator(&iterator);
        if (!flo_html_containsStringHashSet(set2, element)) {
            return HASH_COMPARISON_DIFFERENT_CONTENT;
        }
    }

    flo_html_initStringHashSetIterator(&iterator, set2);

    while (flo_html_hasNextStringHashSetIterator(&iterator)) {
        const flo_html_String element =
            flo_html_nextStringHashSetIterator(&iterator);
        if (!flo_html_containsStringHashSet(set1, element)) {
            return HASH_COMPARISON_DIFFERENT_CONTENT;
        }
    }

    return HASH_COMPARISON_SUCCESS;
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
        if (set->array[iterator->index].indexID > 0) {
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
        if (set->array[iterator->index].indexID > 0) {
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
