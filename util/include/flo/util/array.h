#ifndef FLO_UTIL_ARRAY_H
#define FLO_UTIL_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/util/memory/arena.h"

#define FLO_ARRAY(T)                                                           \
    struct {                                                                   \
        T *buf;                                                                \
        ptrdiff_t len;                                                         \
    }

#define FLO_DYNAMIC_ARRAY(T)                                                   \
    struct {                                                                   \
        T *buf;                                                                \
        ptrdiff_t len;                                                         \
        ptrdiff_t cap;                                                         \
    }

typedef struct {
    char *buf;
    ptrdiff_t len;
    ptrdiff_t cap;
} DASlice;

/**
 * Written assuming that flo_Arena bumps up! Otherwise the middle case statement
 * where we only do a times 1 alloc does not hold.
 */
__attribute((unused)) static void flo_grow(void *slice, ptrdiff_t size,
                                           ptrdiff_t align, flo_Arena *a,
                                           unsigned char flags) {
    DASlice *replica = (DASlice *)slice;

    if (replica->buf == NULL) {
        replica->cap = 1;
        replica->buf = flo_alloc(a, 2 * size, align, replica->cap, flags);
    } else if (a->beg == replica->buf + size * replica->cap) {
        flo_alloc(a, size, 1, replica->cap, flags);
    } else {
        void *data = flo_alloc(a, 2 * size, align, replica->cap, flags);
        memcpy(data, replica->buf, size * replica->len);
        replica->buf = data;
    }

    replica->cap *= 2;
}

#define FLO_COPY_DYNAMIC_ARRAY(newArr, oldArr, t, a)                           \
    newArr.buf = FLO_NEW(a, t, (oldArr).len);                                  \
    memcpy((newArr).buf, (oldArr).buf, (oldArr).len *FLO_SIZEOF(t));           \
    (newArr).len = (oldArr).len;                                               \
    (newArr).cap = (oldArr).len;

#define FLO_PUSH_2(s, a)                                                       \
    ({                                                                         \
        typeof(s) s_ = (s);                                                    \
        typeof(a) a_ = (a);                                                    \
        if (s_->len >= s_->cap) {                                              \
            flo_grow(s_, FLO_SIZEOF(*s_->buf), FLO_ALIGNOF(*s_->buf), a_, 0);  \
        }                                                                      \
        s_->buf + s_->len++;                                                   \
    })
#define FLO_PUSH_3(s, a, f)                                                    \
    ({                                                                         \
        typeof(s) s_ = (s);                                                    \
        typeof(a) a_ = (a);                                                    \
        if (s_->len >= s_->cap) {                                              \
            flo_grow(s_, FLO_SIZEOF(*s_->buf), FLO_ALIGNOF(*s_->buf), a_, f);  \
        }                                                                      \
        s_->buf + s_->len++;                                                   \
    })
#define FLO_PUSH_X(a, b, c, d, ...) d
#define FLO_PUSH(...)                                                          \
    FLO_PUSH_X(__VA_ARGS__, FLO_PUSH_3, FLO_PUSH_2)(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
