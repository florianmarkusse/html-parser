

#include "flo/html-parser/hash/element-id-hash.h"

#ifdef ELEMENT_ID_UINT_16

// https://github.com/skeeto/hash-prospector
// 3-round xorshift-multiply (-Xn3)
// bias = 0.0045976709018820602
uint16_t hash16_xm3(uint16_t x) {
    x ^= x >> 7;
    x *= 0x2993U;
    x ^= x >> 5;
    x *= 0xe877U;
    x ^= x >> 9;
    x *= 0x0235U;
    x ^= x >> 10;
    return x;
}
#endif

HashStatus elementHashSetInit(ElementHashSet *set, const size_t capacity) {
    set->arrayLen = capacity;
    set->entries = 0;
    set->array = calloc(capacity, sizeof(element_id));
    if (set->array == NULL) {
        return HASH_ERROR_MEMORY;
    }
    return HASH_SUCCESS;
}

HashStatus elementHashSetInsert(ElementHashSet *set, const element_id id) {
    if (set->entries >= set->arrayLen) {
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

bool elementHashSetContains(const ElementHashSet *set, const element_id id) {
    size_t index = hash16_xm3(id) % set->arrayLen;

    while (set->array[index] != 0) {
        if (set->array[index] == id) {
            return true;
        }
        index = (index + 1) % set->arrayLen;
    }

    return false;
}

void elementHashSetDestroy(ElementHashSet *set) {
    free(set->array);
    set->arrayLen = 0;
    set->entries = 0;
}

void elementHashSetIteratorInit(ElementHashSetIterator *iterator,
                                const ElementHashSet *set) {
    iterator->set = set;
    iterator->index = 0;
}

element_id elementHashSetIteratorNext(ElementHashSetIterator *iterator) {
    const ElementHashSet *set = iterator->set;

    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index] != 0) {
            return set->array[iterator->index++];
        }
        iterator->index++;
    }

    return 0;
}

bool elementHashSetIteratorHasNext(ElementHashSetIterator *iterator) {
    const ElementHashSet *set = iterator->set;
    while (iterator->index < set->arrayLen) {
        if (set->array[iterator->index] != 0) {
            return true;
        }
        iterator->index++;
    }
    return false;
}

void elementHashSetIteratorReset(ElementHashSetIterator *iterator) {
    iterator->index = 0;
}
