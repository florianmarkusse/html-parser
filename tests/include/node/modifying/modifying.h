#ifndef NODE_MODIFYING_MODIFYING_H
#define NODE_MODIFYING_MODIFYING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

bool testNodeModifications(ptrdiff_t *successes, ptrdiff_t *failures,
                           flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
