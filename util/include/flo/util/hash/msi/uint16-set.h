#ifndef FLO_UTIL_HASH_MSI_UINT16_SET_H
#define FLO_UTIL_HASH_MSI_UINT16_SET_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "common.h"

typedef FLO_MSI_SET(uint16_t) flo_msi_Uint16;

bool flo_msi_insertUint16(uint16_t value, size_t hash, flo_msi_Uint16 *index);

/**
 * Assumes you know what hash function was used in this hash set. If you use the
 * wrong hash, you get wrong answers!!!
 */
bool flo_msi_containsUint16(uint16_t value, size_t hash, flo_msi_Uint16 *index);

#define FLO_FOR_EACH_MSI_UINT16(element, msiSet)                               \
    for (ptrdiff_t _index = 0; _index < (1 << (msiSet)->exp); ++_index)        \
        if (((element) = (msiSet)->buf[_index]) != 0)

#ifdef __cplusplus
}
#endif

#endif
