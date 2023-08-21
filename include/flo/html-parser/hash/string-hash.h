#ifndef FLO_HTML_PARSER_HASH_STRING_HASH_H
#define FLO_HTML_PARSER_HASH_STRING_HASH_H

#include <stdbool.h>
#include <stdlib.h>

#include "flo/html-parser/type/element/elements-container.h"
#include "hash-status.h"

/**
 * Hashing with linear probing for natural values > 0 up until size_t max
 * size.
 */
typedef struct {
    const char **array;
    size_t arrayLen;
    size_t entries;
} __attribute__((aligned(32))) StringHashSet;

HashStatus stringHashSetInit(StringHashSet *set, size_t capacity);

HashStatus stringHashSetInsert(StringHashSet *set, const char *string);

bool stringHashSetContains(const StringHashSet *set, const char *string);

void stringHashSetDestroy(StringHashSet *set);

typedef struct {
    const StringHashSet *set;
    size_t index;
} __attribute__((aligned(16))) StringHashSetIterator;

void stringHashSetIteratorInit(StringHashSetIterator *iterator,
                               const StringHashSet *set);

const char *stringHashSetIteratorNext(StringHashSetIterator *iterator);

bool stringHashSetIteratorHasNext(StringHashSetIterator *iterator);

void stringHashSetIteratorReset(StringHashSetIterator *iterator);

#endif
