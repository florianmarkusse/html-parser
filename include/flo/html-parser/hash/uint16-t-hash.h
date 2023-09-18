#ifndef FLO_HTML_PARSER_HASH_UINT16_T_HASH_H
#define FLO_HTML_PARSER_HASH_UINT16_T_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdlib.h>

#include "flo/html-parser/type/element/elements-container.h"
#include "hash-status.h"

typedef struct {
    uint16_t value;
    uint16_t hash;
} __attribute__((aligned(4))) flo_html_Uint16Entry;

/**
 * Hashing with linear probing for natural values > 0 up until uint_16t max
 * size.
 */
typedef struct {
    flo_html_Uint16Entry *array;
    size_t arrayLen;
    size_t entries;
} __attribute__((aligned(32))) flo_html_Uint16HashSet;

flo_html_HashStatus flo_html_initUint16HashSet(flo_html_Uint16HashSet *set,
                                               uint16_t capacity);

flo_html_HashStatus flo_html_insertUint16HashSet(flo_html_Uint16HashSet *set,
                                                 uint16_t id);

flo_html_HashStatus
flo_html_uint16HashSetToArray(const flo_html_Uint16HashSet *set,
                              uint16_t **results, size_t *resultsLen);

bool flo_html_containsUint16HashSet(const flo_html_Uint16HashSet *set,
                                    uint16_t id);

void flo_html_destroyUint16HashSet(flo_html_Uint16HashSet *set);
flo_html_HashStatus
flo_html_copyUint16HashSet(const flo_html_Uint16HashSet *originalSet,
                           flo_html_Uint16HashSet *copy);
void flo_html_resetUint16HashSet(flo_html_Uint16HashSet *set);

typedef struct {
    const flo_html_Uint16HashSet *set;
    size_t index;
} __attribute__((aligned(16))) flo_html_Uint16HashSetIterator;

void flo_html_initUint16HashSetIterator(
    flo_html_Uint16HashSetIterator *iterator,
    const flo_html_Uint16HashSet *set);

uint16_t
flo_html_nextUint16HashSetIterator(flo_html_Uint16HashSetIterator *iterator);

bool flo_html_hasNextUint16HashSetIterator(
    flo_html_Uint16HashSetIterator *iterator);

void flo_html_resetUint16HashSetIterator(
    flo_html_Uint16HashSetIterator *iterator);

#ifdef __cplusplus
}
#endif

#endif
