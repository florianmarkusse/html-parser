#ifndef FLO_HTML_PARSER_UTIL_HASH_STRING_HASH_H
#define FLO_HTML_PARSER_UTIL_HASH_STRING_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "flo/html-parser/definitions.h"
#include "flo/html-parser/util/memory.h"
#include "flo/html-parser/util/text/string.h"
#include "hash-comparison-status.h"

/**
 * Hashing with linear probing for natural values > 0 up until ptrdiff_t max
 * size.
 */

typedef struct {
    ptrdiff_t entryIndex;
    bool wasInserted;
} flo_html_StringInsert;

typedef struct {
    ptrdiff_t entryIndex;
    size_t hash;
    flo_html_String string;
} flo_html_StringHashEntry;

typedef struct {
    flo_html_StringHashEntry *array;
    ptrdiff_t arrayLen;
    ptrdiff_t entries;
} flo_html_StringHashSet;

flo_html_StringHashSet flo_html_initStringHashSet(ptrdiff_t capacity,
                                                  flo_html_Arena *perm);

flo_html_StringInsert flo_html_insertStringHashSet(flo_html_StringHashSet *set,
                                                   flo_html_String string,
                                                   flo_html_Arena *perm);

ptrdiff_t flo_html_containsStringHashSet(flo_html_StringHashSet *set,
                                         flo_html_String string);

flo_html_HashComparisonStatus
flo_html_equalsStringHashSet(flo_html_StringHashSet *set1,
                             flo_html_StringHashSet *set2);

typedef struct {
    flo_html_StringHashSet *set;
    ptrdiff_t index;
} flo_html_StringHashSetIterator;

flo_html_String
flo_html_nextStringHashSetIterator(flo_html_StringHashSetIterator *iterator);

bool flo_html_hasNextStringHashSetIterator(
    flo_html_StringHashSetIterator *iterator);

#ifdef __cplusplus
}
#endif

#endif
