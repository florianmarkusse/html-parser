#ifndef NODE_QUERYING_QUERYING_H
#define NODE_QUERYING_QUERYING_H

#include <stdbool.h>
#include <stddef.h>

bool testNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures);
bool testBoolNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures);
bool testCharNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures);
bool testArrayNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures);

#endif
