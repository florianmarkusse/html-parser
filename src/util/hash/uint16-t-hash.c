#include <string.h>

#include "flo/html-parser/util/hash/hashes.h"
#include "flo/html-parser/util/hash/uint16-t-hash.h"
#include "flo/html-parser/util/memory.h"

#define MAX_CAPACITY ((1U << 16U) - 1) // Maximum capacity for uint16_t

flo_html_Uint16HashSet flo_html_initUint16HashSet(const uint16_t capacity,
                                                  flo_html_Arena *perm) {
    return (flo_html_Uint16HashSet){
        .array = FLO_HTML_NEW(perm, flo_html_Uint16Entry, capacity,
                              FLO_HTML_ZERO_MEMORY),
        .arrayLen = capacity,
        .entries = 0,
    };
}

bool flo_html_insertUint16HashSet(flo_html_Uint16HashSet *set,
                                  const uint16_t id, flo_html_Arena *perm) {
    uint16_t hash = flo_html_hash16_xm3(id); // Calculate the hash once

    ptrdiff_t index = hash % set->arrayLen;

    while (set->array[index].value != 0) {
        if (set->array[index].value == id) {
            return true;
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
            return false;
        }

        ptrdiff_t newCapacity = (set->arrayLen * 2 <= MAX_CAPACITY)
                                    ? set->arrayLen * 2
                                    : MAX_CAPACITY;
        flo_html_Uint16Entry *oldArray = set->array;
        set->array = FLO_HTML_NEW(perm, flo_html_Uint16Entry, newCapacity,
                                  FLO_HTML_ZERO_MEMORY);

        // Rehashing.
        for (ptrdiff_t i = 0; i < set->arrayLen; i++) {
            if (set->array[i].value != 0) {
                ptrdiff_t newIndex = oldArray[i].hash % newCapacity;
                while (set->array[newIndex].value != 0) {
                    newIndex = (newIndex + 1) % newCapacity;
                }
                set->array[newIndex].value = oldArray[i].value;
                set->array[newIndex].hash = oldArray[i].hash;
            }
        }

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

    return true;
}

flo_html_uint16_t_a
flo_html_uint16HashSetToArray(const flo_html_Uint16HashSet *set,
                              flo_html_Arena *perm) {
    flo_html_uint16_t_a result;
    result.buf = FLO_HTML_NEW(perm, uint16_t, set->entries);
    result.len = set->entries;

    ptrdiff_t resultIndex = 0;
    for (ptrdiff_t i = 0; i < set->arrayLen; i++) {
        if (set->array[i].value != 0) {
            result.buf[resultIndex++] = set->array[i].value;
        }
    }

    return result;
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

flo_html_Uint16HashSet
flo_html_copyUint16HashSet(const flo_html_Uint16HashSet *originalSet,
                           flo_html_Arena *perm) {
    flo_html_Uint16HashSet copy =
        flo_html_initUint16HashSet(originalSet->arrayLen, perm);
    memcpy(copy.array, originalSet->array,
           originalSet->arrayLen * sizeof(flo_html_Uint16Entry));
    copy.entries = originalSet->entries;

    return copy;
}

void flo_html_resetUint16HashSet(flo_html_Uint16HashSet *set) {
    if (set->array != NULL) {
        memset(set->array, 0, set->arrayLen * sizeof(flo_html_Uint16Entry));
    }
    set->entries = 0;
}

flo_html_Uint16HashSetIterator
flo_html_initUint16HashSetIterator(const flo_html_Uint16HashSet *set) {
    return (flo_html_Uint16HashSetIterator){.set = set, .index = 0};
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
