#ifndef FLO_HTML_PARSER_UTIL_MEMORY_H
#define FLO_HTML_PARSER_UTIL_MEMORY_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>

#include "flo/html-parser/util/assert.h"
#include "flo/html-parser/util/error.h"

typedef struct {
    char *beg;
    char *end;
    ptrdiff_t cap;
    void **jmp_buf;
} flo_html_Arena;

__attribute__((unused)) static inline flo_html_Arena
flo_html_newArena(ptrdiff_t cap) {
    flo_html_Arena a;
    a.beg = mmap(NULL, cap, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS,
                 -1, 0);
    if (a.beg == MAP_FAILED) {
        a.beg = NULL;
        a.end = NULL;
        a.cap = 0;
    }
    a.end = a.beg + cap;
    a.cap = cap;
    return a;
}

__attribute__((unused)) static inline void
flo_html_destroyArena(flo_html_Arena *arena) {
    if (munmap(arena->beg, arena->cap) == -1) {
        FLO_HTML_PRINT_ERROR("Failed to unmap memory from arena!\n"
                             "Arena Details:\n"
                             "  beg: %p\n"
                             "  end: %p\n"
                             "  cap: %td\n"
                             "Zeroing Arena regardless.",
                             arena->beg, arena->end, arena->cap);
    }
    arena->cap = 0;
    arena->beg = NULL;
    arena->end = NULL;
}

#define FLO_HTML_ZERO_MEMORY 0x01
#define FLO_HTML_NULL_ON_FAIL 0x02

__attribute((unused, malloc, alloc_size(2, 4),
             alloc_align(3))) static inline void *
flo_html_alloc(flo_html_Arena *a, ptrdiff_t size, ptrdiff_t align,
               ptrdiff_t count, unsigned char flags) {
    FLO_HTML_ASSERT(align > 0);
    FLO_HTML_ASSERT((align & (align - 1)) == 0);

    ptrdiff_t total = size * count;

    char *newEnd = a->end;
    newEnd -= total;
    newEnd -= (uintptr_t)a->end & !(align - 1); // fix alignment.

    if (newEnd < a->beg) {
        if (flags & FLO_HTML_NULL_ON_FAIL) {
            return NULL;
        }
        __builtin_longjmp(a->jmp_buf, 1);
    }

    a->end = newEnd;

    return flags & FLO_HTML_ZERO_MEMORY ? memset(a->end, 0, total) : a->end;
}

__attribute((unused)) static void *
flo_html_copyToArena(flo_html_Arena *arena, void *data, ptrdiff_t size,
                     ptrdiff_t align, ptrdiff_t count) {
    unsigned char *copy = flo_html_alloc(arena, size, align, count, 0);
    memcpy(copy, data, size * count);
    return copy;
}

#define FLO_HTML_SIZEOF(x) (ptrdiff_t)sizeof(x)
#define FLO_HTML_COUNTOF(a) (FLO_HTML_SIZEOF(a) / FLO_HTML_SIZEOF(*(a)))
#define FLO_HTML_LENGTHOF(s) (FLO_HTML_COUNTOF(s) - 1)
#define FLO_HTML_ALIGNOF(t) (_Alignof(t))

#define FLO_HTML_NEW_2(a, t)                                                   \
    (t *)flo_html_alloc(a, FLO_HTML_SIZEOF(t), FLO_HTML_ALIGNOF(t), 1, 0)
#define FLO_HTML_NEW_3(a, t, n)                                                \
    (t *)flo_html_alloc(a, FLO_HTML_SIZEOF(t), FLO_HTML_ALIGNOF(t), n, 0)
#define FLO_HTML_NEW_4(a, t, n, f)                                             \
    (t *)flo_html_alloc(a, FLO_HTML_SIZEOF(t), FLO_HTML_ALIGNOF(t), n, f)
#define FLO_HTML_NEW_X(a, b, c, d, e, ...) e
#define FLO_HTML_NEW(...)                                                      \
    FLO_HTML_NEW_X(__VA_ARGS__, FLO_HTML_NEW_4, FLO_HTML_NEW_3,                \
                   FLO_HTML_NEW_2)                                             \
    (__VA_ARGS__)

#ifdef __cplusplus
}
#endif

#endif
