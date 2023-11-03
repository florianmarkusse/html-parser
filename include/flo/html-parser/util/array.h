#ifndef FLO_HTML_PARSER_UTIL_ARRAY_H
#define FLO_HTML_PARSER_UTIL_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "memory.h"

#define FLO_HTML_ARRAY(T)                                                      \
    struct {                                                                   \
        T *buf;                                                                \
        ptrdiff_t len;                                                         \
    }

#define FLO_HTML_DYNAMIC_ARRAY(T)                                              \
    struct {                                                                   \
        T *buf;                                                                \
        ptrdiff_t len;                                                         \
        ptrdiff_t cap;                                                         \
    }

typedef struct {
    char *buf;
    ptrdiff_t len;
    ptrdiff_t cap;
} Slice;

__attribute((unused)) static void flo_html_grow(void *slice, ptrdiff_t size,
                                                ptrdiff_t align,
                                                flo_html_Arena *a,
                                                unsigned char flags) {
    Slice *replica = (Slice *)slice;

    if (replica->buf == NULL) {
        replica->cap = 1;
        replica->buf = flo_html_alloc(a, 2 * size, align, replica->cap, flags);
    } else if (replica->buf == a->end) {
        void *buf = flo_html_alloc(a, size, align, replica->cap, flags);
        memcpy(buf, replica->buf, size * replica->len);
        replica->buf = buf;
    } else {
        void *buf = flo_html_alloc(a, 2 * size, align, replica->cap, flags);
        memcpy(buf, replica->buf, size * replica->len);
        replica->buf = buf;
    }

    replica->cap *= 2;
}

#define FLO_HTML_COPY_DYNAMIC_ARRAY(newArr, oldArr, t, a)                      \
    newArr.buf = FLO_HTML_NEW(a, t, (oldArr).len);                             \
    memcpy((newArr).buf, (oldArr).buf, (oldArr).len *FLO_HTML_SIZEOF(t));      \
    (newArr).len = (oldArr).len;                                               \
    (newArr).cap = (oldArr).len;

#define FLO_HTML_PUSH_2(s, a)                                                  \
    ({                                                                         \
        typeof(s) s_ = (s);                                                    \
        typeof(a) a_ = (a);                                                    \
        if (s_->len >= s_->cap) {                                              \
            flo_html_grow(s_, FLO_HTML_SIZEOF(*s_->buf),                       \
                          FLO_HTML_ALIGNOF(*s_->buf), a_, 0);                  \
        }                                                                      \
        s_->buf + s_->len++;                                                   \
    })
#define FLO_HTML_PUSH_3(s, a, f)                                               \
    ({                                                                         \
        typeof(s) s_ = (s);                                                    \
        typeof(a) a_ = (a);                                                    \
        if (s_->len >= s_->cap) {                                              \
            flo_html_grow(s_, FLO_HTML_SIZEOF(*s_->buf),                       \
                          FLO_HTML_ALIGNOF(*s_->buf), a_, f);                  \
        }                                                                      \
        s_->buf + s_->len++;                                                   \
    })
#define FLO_HTML_PUSH_X(a, b, c, d, ...) d
#define FLO_HTML_PUSH(...)                                                     \
    FLO_HTML_PUSH_X(__VA_ARGS__, FLO_HTML_PUSH_3, FLO_HTML_PUSH_2)(__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
