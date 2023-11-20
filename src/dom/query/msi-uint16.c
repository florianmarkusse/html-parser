#include "flo/html-parser/dom/query/msi-uint16.h"
#include "hash/hashes.h"
#include <string.h>

void rehashIndex(flo_msi_Uint16 *oldIndex, flo_msi_Uint16 *newIndex) {
    FLO_ASSERT(newIndex->len == 0);
    for (int32_t i = 0; i < (1 << oldIndex->exp); i++) {
        uint16_t value = oldIndex->buf[i];
        if (value != 0) {
            flo_msi_insertUint16(value, flo_hash16_xm3(value), newIndex);
        }
    }
}

bool flo_msi_html_uint16Insert(uint16_t value, flo_msi_Uint16 *index,
                               flo_Arena *perm) {
    if ((uint32_t)index->len >= ((uint32_t)1 << index->exp) / 2) {
        flo_msi_Uint16 newIndex = (flo_msi_Uint16){.exp = index->exp + 1};
        flo_msi_newSet(&newIndex, FLO_SIZEOF(*newIndex.buf),
                       FLO_ALIGNOF(*newIndex.buf), perm);
        rehashIndex(index, &newIndex);
        *index = newIndex;
    }
    return flo_msi_insertUint16(value, flo_hash16_xm3(value), index);
}

void flo_msi_html_resetUint16(flo_msi_Uint16 *set) {
    FLO_ASSERT(set->buf != NULL);

    memset(set->buf, 0, (1 << set->exp) * sizeof(uint16_t));
    set->len = 0;
}

flo_uint16_a flo_msi_html_toArray(flo_msi_Uint16 *index, flo_Arena *perm) {
    flo_uint16_a result;
    result.buf = FLO_NEW(perm, uint16_t, index->len);
    result.len = index->len;

    ptrdiff_t resultIndex = 0;
    uint16_t value;
    FLO_FOR_EACH_MSI_UINT16(value, index) { result.buf[resultIndex++] = value; }

    return result;
}

flo_msi_Uint16 flo_msi_html_copyUint16(flo_msi_Uint16 *index, flo_Arena *perm) {
    FLO_ASSERT(index->buf != NULL);
    flo_msi_Uint16 copy = FLO_NEW_MSI_SET(flo_msi_Uint16, index->exp, perm);
    memcpy(copy.buf, index->buf, (1 << index->exp) * sizeof(uint16_t));
    copy.len = index->len;

    return copy;
}
