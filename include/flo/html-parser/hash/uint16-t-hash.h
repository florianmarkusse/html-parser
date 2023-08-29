#ifndef FLO_HTML_PARSER_HASH_UINT16_T_HASH_H
#define FLO_HTML_PARSER_HASH_UINT16_T_HASH_H

#include <stdbool.h>
#include <stdlib.h>

#include "flo/html-parser/type/element/elements-container.h"
#include "hash-status.h"

typedef struct {
    uint16_t value;
    uint16_t hash;
} __attribute__((aligned(4))) Uint16Entry;

/**
 * Hashing with linear probing for natural values > 0 up until uint_16t max
 * size.
 */
typedef struct {
    Uint16Entry *array;
    size_t arrayLen;
    size_t entries;
} __attribute__((aligned(32))) Uint16HashSet;

HashStatus initUint16HashSet(Uint16HashSet *set, uint16_t capacity);

HashStatus insertUint16HashSet(Uint16HashSet *set, uint16_t id);

HashStatus uint16HashSetToArray(const Uint16HashSet *set, uint16_t **results,
                                size_t *resultsLen);

bool containsUint16HashSet(const Uint16HashSet *set, uint16_t id);

void destroyUint16HashSet(Uint16HashSet *set);
HashStatus copyUint16HashSet(const Uint16HashSet *originalSet,
                             Uint16HashSet *copy);
void resetUint16HashSet(Uint16HashSet *set);

typedef struct {
    const Uint16HashSet *set;
    size_t index;
} __attribute__((aligned(16))) Uint16HashSetIterator;

void initUint16HashSetIterator(Uint16HashSetIterator *iterator,
                               const Uint16HashSet *set);

uint16_t nextUint16HashSetIterator(Uint16HashSetIterator *iterator);

bool hasNextUint16HashSetIterator(Uint16HashSetIterator *iterator);

void resetUint16HashSetIterator(Uint16HashSetIterator *iterator);

#endif
