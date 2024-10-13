#ifndef FLO_UTIL_MEMORY_ARENA_H
#define FLO_UTIL_MEMORY_ARENA_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "assert.h"
#include "error.h"
#include "macros.h"

typedef struct {
    char *beg;
    char *end;
    ptrdiff_t cap;
    void **jmp_buf;
} flo_Arena;

__attribute((malloc, alloc_size(2, 4), alloc_align(3))) void *
flo_alloc(flo_Arena *a, ptrdiff_t size, ptrdiff_t align, ptrdiff_t count,
          unsigned char flags);

__attribute((malloc, alloc_size(3, 5), alloc_align(4))) void *
flo_copyToArena(flo_Arena *arena, void *data, ptrdiff_t size, ptrdiff_t align,
                ptrdiff_t count);

#define FLO_NEW_2(a, t) (t *)flo_alloc(a, FLO_SIZEOF(t), FLO_ALIGNOF(t), 1, 0)
#define FLO_NEW_3(a, t, n)                                                     \
    (t *)flo_alloc(a, FLO_SIZEOF(t), FLO_ALIGNOF(t), n, 0)
#define FLO_NEW_4(a, t, n, f)                                                  \
    (t *)flo_alloc(a, FLO_SIZEOF(t), FLO_ALIGNOF(t), n, f)
#define FLO_NEW_X(a, b, c, d, e, ...) e
#define FLO_NEW(...)                                                           \
    FLO_NEW_X(__VA_ARGS__, FLO_NEW_4, FLO_NEW_3, FLO_NEW_2)                    \
    (__VA_ARGS__)

#endif
