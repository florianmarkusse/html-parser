#ifndef NODE_DELETING_DELETING_H
#define NODE_DELETING_DELETING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

bool testNodeDeletions(ptrdiff_t *successes, ptrdiff_t *failures,
                       flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
