#include <stdio.h>
#include <string.h>

#include "flo/html-parser/util/hash/hash-element.h"
#include "flo/html-parser/util/hash/hashes.h"
#include "flo/html-parser/util/hash/string-hash.h"
#include "flo/html-parser/util/memory.h"

#define MAX_CAPACITY ((1U << 18U) - 1) // Maximum capacity for uint16_t

#define MAX_PROBES (1U << 4U)

flo_html_StringHashSet flo_html_initStringHashSet(const ptrdiff_t capacity,
                                                  flo_html_Arena *perm) {
    return (flo_html_StringHashSet){
        .arrayLen = capacity,
        .entries = 0,
        .array = FLO_HTML_NEW(perm, flo_html_StringHashEntry, capacity,
                              FLO_HTML_ZERO_MEMORY)};
}

// Sets the flo_html_index_id that is used in the DOM, starting at 1 because
// then 0 can be used as an error/init value.
// TODO: Add a way to grow dynamically here !!!
ptrdiff_t flo_html_insertStringAtHash(flo_html_StringHashSet *set,
                                      const flo_html_String string,
                                      const flo_html_HashElement *hashElement) {
    if (set->entries >= set->arrayLen) {
        FLO_HTML_PRINT_ERROR("String hash set is at full capacity!\n");
        return 0;
    }

    set->entries++;

    const size_t arrayIndex =
        (hashElement->hash + hashElement->offset) % set->arrayLen;
    set->array[arrayIndex].string = string;
    set->array[arrayIndex].contains.hashElement = *hashElement;
    set->array[arrayIndex].contains.entryIndex = set->entries;
    return set->entries;
}

// TODO: could improve this to check first if it contains before resizing???
ptrdiff_t flo_html_insertStringHashSet(flo_html_StringHashSet *set,
                                       const flo_html_String string,
                                       flo_html_Arena *perm) {
    size_t hash = flo_html_hashString(string);

    if (set->entries >= set->arrayLen * 0.7) {
        // See if it makes sense to grow.
        if (set->arrayLen >= MAX_CAPACITY * 0.9) {
            FLO_HTML_PRINT_ERROR(
                "Hash set capacity would exceed the maximum capacity: %d!\n",
                MAX_CAPACITY);
            return 0;
        }

        ptrdiff_t newCapacity = (set->arrayLen * 2 <= MAX_CAPACITY)
                                    ? set->arrayLen * 2
                                    : MAX_CAPACITY;
        flo_html_StringHashEntry *oldArray = set->array;
        set->array = FLO_HTML_NEW(perm, flo_html_StringHashEntry, newCapacity,
                                  FLO_HTML_ZERO_MEMORY);

        // Rehashing.
        for (ptrdiff_t i = 0; i < set->arrayLen; i++) {
            ptrdiff_t probes = 0;
            if (set->array[i].string.buf != NULL) {
                ptrdiff_t newIndex =
                    oldArray[i].contains.hashElement.hash % newCapacity;
                while (set->array[newIndex].string.buf != NULL) {
                    probes++;
                    newIndex = (newIndex + probes) % newCapacity;
                }
                set->array[newIndex].string = oldArray[i].string;
                set->array[newIndex].contains.entryIndex =
                    oldArray[i].contains.entryIndex;
                set->array[newIndex].contains.hashElement.hash = hash;
                set->array[newIndex].contains.hashElement.offset = probes;
            }
        }

        set->arrayLen = newCapacity;
    }

    ptrdiff_t index = hash % set->arrayLen;
    ptrdiff_t probes = 0;
    while (set->array[index].string.buf != NULL) {
        if (flo_html_stringEquals(set->array[index].string, string)) {
            return set->array[index].contains.entryIndex;
        }
        probes++;
        index = (index + probes) % set->arrayLen;
    }

    set->entries++;

    set->array[index].string = string;
    set->array[index].contains.entryIndex = set->entries;
    set->array[index].contains.hashElement.hash = hash;
    set->array[index].contains.hashElement.offset = probes;

    return set->entries;
}

flo_html_Contains
flo_html_containsStringHashSet(const flo_html_StringHashSet *set,
                               const flo_html_String string) {
    flo_html_Contains result;

    size_t hash = flo_html_hashString(string);
    result.hashElement.hash = hash;

    ptrdiff_t index = hash % set->arrayLen;

    ptrdiff_t probes = 0;
    while (set->array[index].string.buf != NULL) {
        flo_html_StringHashEntry entry = set->array[index];
        if (flo_html_stringEquals(entry.string, string)) {
            result.hashElement.offset = probes;
            result.entryIndex = entry.contains.entryIndex;
            return result;
        }
        probes++;
        index = (index + 1) % set->arrayLen;
    }

    result.hashElement.offset = probes;
    result.entryIndex = 0;
    return result;
}

const flo_html_String
flo_html_getStringFromHashSet(const flo_html_StringHashSet *set,
                              const flo_html_HashElement *hashElement) {
    return set
        ->array[((hashElement->hash + hashElement->offset) % set->arrayLen)]
        .string;
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
        if (!flo_html_containsStringHashSet(set2, element).entryIndex) {
            return HASH_COMPARISON_DIFFERENT_CONTENT;
        }
    }

    flo_html_initStringHashSetIterator(&iterator, set2);

    while (flo_html_hasNextStringHashSetIterator(&iterator)) {
        const flo_html_String element =
            flo_html_nextStringHashSetIterator(&iterator);
        if (!flo_html_containsStringHashSet(set1, element).entryIndex) {
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
        if (set->array[iterator->index].string.buf != NULL) {
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
        if (set->array[iterator->index].string.buf != NULL) {
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
