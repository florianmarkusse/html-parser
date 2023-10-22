#include <stdio.h>
#include <string.h>

#include "flo/html-parser/util/hash/hash-element.h"
#include "flo/html-parser/util/hash/hashes.h"
#include "flo/html-parser/util/hash/string-hash.h"
#include "flo/html-parser/util/memory.h"

#define MAX_CAPACITY ((1U << 18U) - 1)

#define MAX_PROBES (1U << 4U)

flo_html_StringHashSet flo_html_initStringHashSet(const ptrdiff_t capacity,
                                                  flo_html_Arena *perm) {
    return (flo_html_StringHashSet){
        .arrayLen = capacity,
        .entries = 0,
        .array = FLO_HTML_NEW(perm, flo_html_StringHashEntry, capacity,
                              FLO_HTML_ZERO_MEMORY)};
}

ptrdiff_t flo_html_insertStringHashSet(flo_html_StringHashSet *set,
                                       const flo_html_String string,
                                       flo_html_Arena *perm) {
    size_t newStringHash = flo_html_hashString(string);
    ptrdiff_t newStringProbes = 0;

    while (set->array[(newStringHash + newStringProbes) % set->arrayLen]
               .string.buf != NULL) {
        flo_html_StringHashEntry entry =
            set->array[(newStringHash + newStringProbes) % set->arrayLen];
        if (flo_html_stringEquals(entry.string, string)) {
            return entry.contains.entryIndex;
        }
        if (newStringProbes < MAX_PROBES) {
            newStringProbes++;
        } else {
            FLO_HTML_PRINT_ERROR("Maximum number of probes %u reached!",
                                 MAX_PROBES);
            return 0;
        }
    }

    bool didResize = false;
    if (set->entries >= set->arrayLen * 0.7) {
        didResize = true;
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
            if (set->array[i].string.buf != NULL) {
                ptrdiff_t probes = 0;
                ptrdiff_t hash = oldArray[i].contains.hashElement.hash;
                while (set->array[(hash + probes) % newCapacity].string.buf !=
                       NULL) {
                    probes++;
                }
                ptrdiff_t finalIndex = (hash + probes) % newCapacity;
                set->array[finalIndex].string = oldArray[i].string;
                set->array[finalIndex].contains.entryIndex =
                    oldArray[i].contains.entryIndex;
                set->array[finalIndex].contains.hashElement.hash = hash;
                set->array[finalIndex].contains.hashElement.offset = probes;
            }
        }

        set->arrayLen = newCapacity;
    }

    if (didResize) {
        newStringProbes = 0;
        while (set->array[(newStringHash + newStringProbes) % set->arrayLen]
                   .string.buf != NULL) {
            // Don't need to check for equality here since we already did that.
            if (newStringProbes < MAX_PROBES) {
                newStringProbes++;
            } else {
                FLO_HTML_PRINT_ERROR("Maximum number of probes %u reached!",
                                     MAX_PROBES);
                return 0;
            }
        }
    }

    // increment entries before setting the entry index - 0 used as not found.
    set->entries++;

    ptrdiff_t finalIndex = (newStringHash + newStringProbes) % set->arrayLen;
    set->array[finalIndex] = (flo_html_StringHashEntry){
        .string = string,
        .contains = (flo_html_Contains){
            .entryIndex = set->entries,
            .hashElement = (flo_html_HashElement){.hash = newStringHash,
                                                  .offset = newStringProbes}}};

    return set->entries;
}

flo_html_Contains
flo_html_containsStringHashSet(const flo_html_StringHashSet *set,
                               const flo_html_String string) {
    size_t hash = flo_html_hashString(string);

    ptrdiff_t probes = 0;
    while (set->array[(hash + probes) % set->arrayLen].string.buf != NULL) {
        flo_html_StringHashEntry entry =
            set->array[(hash + probes) % set->arrayLen];
        if (flo_html_stringEquals(entry.string, string)) {
            return entry.contains;
        }
        probes++;
    }

    return (flo_html_Contains){
        .hashElement = (flo_html_HashElement){.hash = hash, .offset = probes},
        .entryIndex = 0};
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

    flo_html_String element;

    flo_html_StringHashSetIterator iterator =
        (flo_html_StringHashSetIterator){.set = set1, .index = 0};
    while ((element = flo_html_nextStringHashSetIterator(&iterator)).len != 0) {
        if (!flo_html_containsStringHashSet(set2, element).entryIndex) {
            return HASH_COMPARISON_DIFFERENT_CONTENT;
        }
    }

    iterator = (flo_html_StringHashSetIterator){.set = set2, .index = 0};
    while ((element = flo_html_nextStringHashSetIterator(&iterator)).len != 0) {
        if (!flo_html_containsStringHashSet(set1, element).entryIndex) {
            return HASH_COMPARISON_DIFFERENT_CONTENT;
        }
    }

    return HASH_COMPARISON_SUCCESS;
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
