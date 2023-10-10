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

void *flo_html_copyToArena(flo_html_Arena *arena, void *data, ptrdiff_t size,
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
    (typeof(t))flo_html_alloc(a, FLO_HTML_SIZEOF(*(t)),                        \
                              FLO_HTML_ALIGNOF(*(t)), 1, 0)
#define FLO_HTML_NEW_3(a, t, n)                                                \
    (t *)flo_html_alloc(a, FLO_HTML_SIZEOF(t), FLO_HTML_ALIGNOF(t), n, 0)
#define FLO_HTML_NEW_4(a, t, n, f)                                             \
    (t *)flo_html_alloc(a, FLO_HTML_SIZEOF(t), FLO_HTML_ALIGNOF(t), n, f)
#define FLO_HTML_NEW_X(a, b, c, d, e, ...) e
#define FLO_HTML_NEW(...)                                                      \
    FLO_HTML_NEW_X(__VA_ARGS__, FLO_HTML_NEW_4, FLO_HTML_NEW_3,                \
                   FLO_HTML_NEW_2)                                             \
    (__VA_ARGS__)

__attribute((unused)) static void flo_html_grow(void *slice, ptrdiff_t size,
                                                ptrdiff_t align,
                                                flo_html_Arena *a,
                                                unsigned char flags) {
    struct {
        char *data;
        ptrdiff_t len;
        ptrdiff_t cap;
    } replica;
    memcpy(&replica, slice, FLO_HTML_SIZEOF(replica));

    if (replica.data == NULL) {
        replica.cap = 1;
        replica.data = flo_html_alloc(a, 2 * size, align, replica.cap, flags);
    } else if (a->end == replica.data - size * replica.cap) {
        flo_html_alloc(a, size, 1, replica.cap, flags);
    } else {
        void *data = flo_html_alloc(a, 2 * size, align, replica.cap, flags);
        memcpy(data, replica.data, size * replica.len);
        replica.data = data;
    }

    replica.cap *= 2;
    memcpy(slice, &replica, sizeof(replica));
}

#define FLO_HTML_PUSH_2(s, a)                                                  \
    ({                                                                         \
        typeof(s) s_ = (s);                                                    \
        typeof(a) a_ = (a);                                                    \
        if (s_->len >= s_->cap) {                                              \
            flo_html_grow(s_, FLO_HTML_SIZEOF(*s_->data),                      \
                          FLO_HTML_ALIGNOF(*s_->data), a_, 0);                 \
        }                                                                      \
        s_->data + s_->len++;                                                  \
    })
#define FLO_HTML_PUSH_3(s, a, f)                                               \
    ({                                                                         \
        typeof(s) s_ = (s);                                                    \
        typeof(a) a_ = (a);                                                    \
        if (s_->len >= s_->cap) {                                              \
            flo_html_grow(s_, FLO_HTML_SIZEOF(*s_->data),                      \
                          FLO_HTML_ALIGNOF(*s_->data), a_, f);                 \
        }                                                                      \
        s_->data + s_->len++;                                                  \
    })
#define FLO_HTML_PUSH_X(a, b, c, d, ...) d
#define FLO_HTML_PUSH(...)                                                     \
    FLO_HTML_NEW_X(__VA_ARGS__, FLO_HTML_PUSH_3, FLO_HTML_NEW_2)(__VA_ARGS__)

/**
 * @brief Free a pointer and set it to NULL.
 *
 * This macro frees the memory pointed to by the given pointer and sets the
 * pointer itself to NULL. It is a safe way to free memory and avoid
 * dereferencing a dangling pointer.
 *
 * @param[in,out] ptr   A pointer to be freed and set to NULL.
 */
//#define FLO_HTML_FREE_TO_NULL(ptr)                                             \
//    do {                                                                       \
//        free(ptr);                                                             \
//        (ptr) = NULL;                                                          \
//    } while (0)

/**
 * @brief Resize a dynamically allocated array.
 *
 * This inline function resizes a dynamically allocated array when the current
 * length exceeds the current capacity. It ensures that enough memory is
 * allocated for the array to hold additional elements. If the reallocation
 * fails, it prints an error message.
 *
 * @param[in]       array           The current array to be resized.
 * @param[in]       currentLen      The current length of the array.
 * @param[in,out]   currentCap      The current capacity of the array (updated
 *                                  if reallocation occurs).
 * @param[in]       elementSize     The size of each element in the array.
 * @param[in]       extraElements   The number of extra elements to accommodate.
 *
 * @return  A pointer to the resized array, or NULL if reallocation fails.
 */
//__attribute__((unused)) static inline void *
// flo_html_resizeArray(void *array, ptrdiff_t currentLen, ptrdiff_t
// *currentCap,
//                     ptrdiff_t elementSize, ptrdiff_t extraElements) {
//    if (currentLen < *currentCap) {
//        return array;
//    }
//    ptrdiff_t newCap = (currentLen + extraElements);
//    void *newArray = realloc(array, newCap * elementSize);
//    if (newArray == NULL) {
//        FLO_HTML_PRINT_ERROR(
//            "Failed to reallocate more memory for the array.\n");
//        return NULL;
//    }
//    *currentCap = newCap;
//    return newArray;
//}

#ifdef __cplusplus
}
#endif

#endif
