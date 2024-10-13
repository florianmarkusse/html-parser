#ifndef FLO_UTIL_HASH_MSI_STRING_SET_H
#define FLO_UTIL_HASH_MSI_STRING_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include "common.h"
#include "flo/util/hash/hash-comparison-status.h"
#include "string.h"

typedef FLO_MSI_SET(flo_String) flo_msi_String;

bool flo_msi_insertString(flo_String string, size_t hash,
                          flo_msi_String *index);

/**
 * Assumes you know what hash function was used in this hash set. If you use the
 * wrong hash, you get wrong answers!!!
 */
bool flo_msi_containsString(flo_String string, size_t hash,
                            flo_msi_String *index);

/**
 * Uses flo_hashStringDjb2 to compare so if any of the sets used a customs
 * hashing function, do not use!!!
 */
flo_HashComparisonStatus flo_msi_equalsStringSet(flo_msi_String *set1,
                                                 flo_msi_String *set2);

#define FLO_FOR_EACH_MSI_STRING(element, msiSet)                               \
    for (ptrdiff_t _index = 0; _index < (1 << (msiSet)->exp); ++_index)        \
        if (((element) = (msiSet)->buf[_index]).len != 0)

// Below an example of rehashing with the old set and a growth factor of 0.5.
//
//
// void rehashIndex(flo_MSI_String *oldIndex,
//                  flo_MSI_String *newIndex) {
//     FLO_ASSERT(newIndex->len == 0);
//     for (int32_t i = 0; i < (1 << oldIndex->exp); i++) {
//         flo_String s = oldIndex->buf[i];
//         if (s.len > 0) {
//             flo_indexInsert(s, flo_hashStringDjb2(s), newIndex);
//         }
//     }
// }
//
// flo_HashEntry indexInsert(flo_String string, flo_String_HashIndex *index,
//                           flo_Arena *perm) {
//     if ((uint32_t)index->len >= ((uint32_t)1 << index->exp) / 2) {
//         flo_String_HashIndex newIndex =
//             (flo_String_HashIndex){.exp = index->exp + 1};
//         flo_newMSISet(&newIndex, FLO_SIZEOF(*newIndex.buf),
//                       FLO_ALIGNOF(*newIndex.buf), perm);
//         rehashIndex(index, &newIndex);
//         *index = newIndex;
//     }
//     return flo_indexInsert(string, flo_hashStringDjb2(string), index);
// }

#ifdef __cplusplus
}
#endif

#endif
