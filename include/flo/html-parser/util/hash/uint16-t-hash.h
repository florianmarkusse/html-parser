#ifndef FLO_HTML_PARSER_UTIL_HASH_UINT16_T_HASH_H
#define FLO_HTML_PARSER_UTIL_HASH_UINT16_T_HASH_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>

#include "flo/html-parser/util/array.h"
#include "flo/html-parser/util/memory.h"

typedef FLO_HTML_ARRAY(uint16_t) flo_html_uint16_t_a;

typedef struct {
    uint16_t value;
    uint16_t hash;
    uint8_t probes;
} flo_html_Uint16Entry;

/**
 * Hashing with linear probing for natural values > 0 up until uint_16t max
 * size.
 */
typedef struct {
    flo_html_Uint16Entry *array;
    ptrdiff_t arrayLen;
    ptrdiff_t entries;
} flo_html_Uint16HashSet;

flo_html_Uint16HashSet flo_html_initUint16HashSet(uint16_t capacity,
                                                  flo_html_Arena *perm);

bool flo_html_insertUint16HashSet(flo_html_Uint16HashSet *set, uint16_t id,
                                  flo_html_Arena *perm);
flo_html_uint16_t_a
flo_html_uint16HashSetToArray(const flo_html_Uint16HashSet *set,
                              flo_html_Arena *perm);

bool flo_html_containsUint16HashSet(const flo_html_Uint16HashSet *set,
                                    uint16_t id);

flo_html_Uint16HashSet
flo_html_copyUint16HashSet(const flo_html_Uint16HashSet *originalSet,
                           flo_html_Arena *perm);
void flo_html_resetUint16HashSet(flo_html_Uint16HashSet *set);

typedef struct {
    const flo_html_Uint16HashSet *set;
    ptrdiff_t index;
} flo_html_Uint16HashSetIterator;

uint16_t
flo_html_nextUint16HashSetIterator(flo_html_Uint16HashSetIterator *iterator);

bool flo_html_hasNextUint16HashSetIterator(
    flo_html_Uint16HashSetIterator *iterator);

#ifdef __cplusplus
}
#endif

#endif
