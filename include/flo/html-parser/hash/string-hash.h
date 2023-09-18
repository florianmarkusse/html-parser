#ifndef FLO_HTML_PARSER_HASH_STRING_HASH_H
#define FLO_HTML_PARSER_HASH_STRING_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "flo/html-parser/comparison-status.h"
#include "flo/html-parser/type/data/definitions.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "hash-element.h"
#include "hash-status.h"

typedef struct {
    flo_html_indexID flo_html_indexID;
    const char *string;
} __attribute__((aligned(16))) flo_html_HashEntry;

/**
 * Hashing with linear probing for natural values > 0 up until size_t max
 * size.
 */
typedef struct {
    flo_html_HashEntry *array;
    size_t arrayLen;
    size_t entries;
} __attribute__((aligned(32))) flo_html_StringHashSet;

flo_html_HashStatus flo_html_initStringHashSet(flo_html_StringHashSet *set,
                                               size_t capacity);

flo_html_HashStatus flo_html_insertStringHashSet(flo_html_StringHashSet *set,
                                                 const char *string);

flo_html_HashStatus
flo_html_insertStringAtHash(flo_html_StringHashSet *set, const char *string,
                            const flo_html_HashElement *hashElement,
                            flo_html_indexID *flo_html_indexID);

bool flo_html_containsStringHashSet(const flo_html_StringHashSet *set,
                                    const char *string);
bool flo_html_containsStringWithDataHashSet(const flo_html_StringHashSet *set,
                                            const char *string,
                                            flo_html_HashElement *hashElement,
                                            flo_html_indexID *flo_html_indexID);
const char *
flo_html_getStringFromHashSet(const flo_html_StringHashSet *set,
                              const flo_html_HashElement *hashElement);

flo_html_ComparisonStatus
flo_html_equalsStringHashSet(const flo_html_StringHashSet *set1,
                             const flo_html_StringHashSet *set2);

void flo_html_destroyStringHashSet(flo_html_StringHashSet *set);

typedef struct {
    const flo_html_StringHashSet *set;
    size_t index;
} __attribute__((aligned(16))) flo_html_StringHashSetIterator;

void flo_html_initStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator,
    const flo_html_StringHashSet *set);

const char *
flo_html_nextStringHashSetIterator(flo_html_StringHashSetIterator *iterator);

bool flo_html_hasNextStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator);

void flo_html_resetStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator);

#ifdef __cplusplus
}
#endif

#endif
