#ifndef FLO_HTML_PARSER_HASH_ELEMENT_ID_HASH_H
#define FLO_HTML_PARSER_HASH_ELEMENT_ID_HASH_H

#include <stdbool.h>
#include <stdlib.h>

#include "flo/html-parser/type/element/elements-container.h"
#include "hash-status.h"

/**
 * Hashing with linear probing for natural values > 0 up until uint_16t max
 * size.
 */
typedef struct {
    element_id *array;
    size_t arrayLen;
    size_t entries;
} __attribute__((aligned(32))) ElementHashSet;

HashStatus elementHashSetInit(ElementHashSet *set, size_t capacity);

HashStatus elementHashSetInsert(ElementHashSet *set, element_id id);

bool elementHashSetContains(const ElementHashSet *set, element_id id);

void elementHashSetDestroy(ElementHashSet *set);

typedef struct {
    const ElementHashSet *set;
    size_t index;
} __attribute__((aligned(16))) ElementHashSetIterator;

void elementHashSetIteratorInit(ElementHashSetIterator *iterator,
                                const ElementHashSet *set);

element_id elementHashSetIteratorNext(ElementHashSetIterator *iterator);

bool elementHashSetIteratorHasNext(ElementHashSetIterator *iterator);

void elementHashSetIteratorReset(ElementHashSetIterator *iterator);

#endif
