#include "flo/util/hash/msi/uint16-set.h"

bool flo_msi_insertUint16(uint16_t value, size_t hash, flo_msi_Uint16 *index) {
    for (int32_t i = (int32_t)hash;;) {
        i = flo_indexLookup(hash, index->exp, i);
        if (index->buf[i] == 0) {
            index->len++;
            index->buf[i] = value;
            return true;
        } else if (index->buf[i] == value) {
            return false;
        }
    }
}

bool flo_msi_containsUint16(uint16_t value, size_t hash,
                            flo_msi_Uint16 *index) {
    for (int32_t i = (int32_t)hash;;) {
        i = flo_indexLookup(hash, index->exp, i);
        if (index->buf[i] == 0) {
            return false;
        } else if (index->buf[i] == value) {
            return true;
        }
    }
}
