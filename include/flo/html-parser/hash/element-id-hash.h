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

HashStatus initElementHashSet(ElementHashSet *set, size_t capacity);

HashStatus insertElementHashSet(ElementHashSet *set, element_id id);

bool containsElementHashSet(const ElementHashSet *set, element_id id);

void destroyElementHashSet(ElementHashSet *set);

typedef struct {
    const ElementHashSet *set;
    size_t index;
} __attribute__((aligned(16))) ElementHashSetIterator;

void initElementHashSetIterator(ElementHashSetIterator *iterator,
                                const ElementHashSet *set);

element_id nextElementHashSetIterator(ElementHashSetIterator *iterator);

bool hasNextElementHashSetIterator(ElementHashSetIterator *iterator);

void resetElementHashSetIterator(ElementHashSetIterator *iterator);

#endif
