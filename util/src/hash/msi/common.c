#include "flo/util/hash/msi/common.h"

/**
 * Written assuming that flo_Arena bumps up! Otherwise the middle case statement
 * where we only do a times 1 alloc does not hold.
 */
void flo_msi_newSet(void *setSlice, ptrdiff_t size, ptrdiff_t align,
                    flo_Arena *a) {
    SetSlice *replica = (SetSlice *)setSlice;
    FLO_ASSERT(replica->exp > 0);

    if (replica->exp >= 31) {
        FLO_ASSERT(false);
        __builtin_longjmp(a->jmp_buf, 1);
    }

    ptrdiff_t cap = 1 << replica->exp;

    if (replica->buf == NULL) {
        replica->buf = flo_alloc(a, size, align, cap, FLO_ZERO_MEMORY);
    } else if (a->beg == replica->buf + size * cap) {
        memset(replica->buf, 0, size * cap);
        flo_alloc(a, size, 1, cap, FLO_ZERO_MEMORY);
        replica->exp++;
        replica->len = 0;
    } else {
        void *data = flo_alloc(a, 2 * size, align, cap, FLO_ZERO_MEMORY);
        replica->buf = data;
        replica->exp++;
        replica->len = 0;
    }
}
