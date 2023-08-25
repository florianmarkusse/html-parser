#ifndef FLO_HTML_PARSER_HASH_STRING_HASH_H
#define FLO_HTML_PARSER_HASH_STRING_HASH_H

#include <stdbool.h>
#include <stdlib.h>

#include "flo/html-parser/type/data/definitions.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "hash-element.h"
#include "hash-status.h"

typedef struct {
    indexID indexID;
    const char *string;
} __attribute__((aligned(16))) HashEntry;

/**
 * Hashing with linear probing for natural values > 0 up until size_t max
 * size.
 */
typedef struct {
    HashEntry *array;
    size_t arrayLen;
    size_t entries;
} __attribute__((aligned(32))) StringHashSet;

HashStatus initStringHashSet(StringHashSet *set, size_t capacity);

HashStatus insertStringHashSet(StringHashSet *set, const char *string);

HashStatus insertStringWithDataHashSet(StringHashSet *set, const char *string,
                                       HashElement *hashElement,
                                       indexID *indexID);

bool containsStringHashSet(const StringHashSet *set, const char *string);
bool containsStringWithDataHashSet(const StringHashSet *set, const char *string,
                                   indexID *indexID);
const char *getStringFromHashSet(const StringHashSet *set,
                                 const HashElement *hashElement);

void destroyStringHashSet(StringHashSet *set);

typedef struct {
    const StringHashSet *set;
    size_t index;
} __attribute__((aligned(16))) StringHashSetIterator;

void initStringHashSetIterator(StringHashSetIterator *iterator,
                               const StringHashSet *set);

const char *nextStringHashSetIterator(StringHashSetIterator *iterator);

bool hasNextStringHashSetIterator(StringHashSetIterator *iterator);

void resetStringHashSetIterator(StringHashSetIterator *iterator);

#endif
