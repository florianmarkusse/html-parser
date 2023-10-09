#ifndef FLO_HTML_PARSER_UTIL_HASH_STRING_HASH_H
#define FLO_HTML_PARSER_UTIL_HASH_STRING_HASH_H

#include "flo/html-parser/util/memory.h"
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/util/text/string.h"
#include "hash-comparison-status.h"
#include "hash-element.h"
#include "hash-status.h"

typedef struct {
    flo_html_index_id indexID;
    flo_html_String string;
} flo_html_StringHashEntry;

/**
 * Hashing with linear probing for natural values > 0 up until ptrdiff_t max
 * size.
 */
typedef struct {
    flo_html_StringHashEntry *array;
    ptrdiff_t arrayLen;
    ptrdiff_t entries;
} flo_html_StringHashSet;

typedef struct {
    flo_html_HashElement hashElement;
    flo_html_index_id entryID;
} flo_html_HashEntry;

typedef struct {
    bool wasPresent;
    flo_html_HashEntry hashEntry;
} flo_html_Contains;

void flo_html_initStringHashSet(flo_html_StringHashSet *set, ptrdiff_t capacity,
                                flo_html_Arena *perm);

flo_html_HashStatus flo_html_insertStringHashSet(flo_html_StringHashSet *set,
                                                 const flo_html_String string);

flo_html_index_id
flo_html_insertStringAtHash(flo_html_StringHashSet *set,
                            const flo_html_String string,
                            const flo_html_HashElement *hashElement);

bool flo_html_containsStringHashSet(const flo_html_StringHashSet *set,
                                    const flo_html_String string);
flo_html_Contains
flo_html_containsStringWithDataHashSet(const flo_html_StringHashSet *set,
                                       const flo_html_String string);

const flo_html_String
flo_html_getStringFromHashSet(const flo_html_StringHashSet *set,
                              const flo_html_HashElement *hashElement);

flo_html_HashComparisonStatus
flo_html_equalsStringHashSet(const flo_html_StringHashSet *set1,
                             const flo_html_StringHashSet *set2);

void flo_html_destroyStringHashSet(flo_html_StringHashSet *set);

typedef struct {
    const flo_html_StringHashSet *set;
    ptrdiff_t index;
} flo_html_StringHashSetIterator;

void flo_html_initStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator,
    const flo_html_StringHashSet *set);

const flo_html_String
flo_html_nextStringHashSetIterator(flo_html_StringHashSetIterator *iterator);

bool flo_html_hasNextStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator);

void flo_html_resetStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator);

#ifdef __cplusplus
}
#endif

#endif
