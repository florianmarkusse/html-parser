#ifndef FLO_UTIL_MEMORY_POOL_ALLOCATOR_H
#define FLO_UTIL_MEMORY_POOL_ALLOCATOR_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>

#include "assert.h"
#include "macros.h"

struct flo_PoolHead {
    struct flo_PoolHead *next;
};
typedef struct flo_PoolHead flo_PoolHead;

typedef struct {
    char *beg;
    ptrdiff_t cap;
    ptrdiff_t chunkSize;

    flo_PoolHead *head;

    void **jmp_buf;
} flo_PoolAllocator;

void flo_freePool(flo_PoolAllocator *pool);

/*
 * Set up the pool allocator values, except for the jmp_buf!
 */
flo_PoolAllocator flo_createPoolAllocator(char *buffer, ptrdiff_t cap,
                                          ptrdiff_t chunkSize);

__attribute((malloc)) void *flo_poolAlloc(flo_PoolAllocator *pool,
                                          unsigned char flags);

void flo_freePoolNode(flo_PoolAllocator *pool, void *ptr);

#ifdef __cplusplus
}
#endif

#endif
