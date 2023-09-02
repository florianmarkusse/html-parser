#ifndef NODE_QUERYING_QUERYING_H
#define NODE_QUERYING_QUERYING_H

#include <stdbool.h>
#include <stdlib.h>

bool testNodeQueries(size_t *successes, size_t *failures);
bool testBoolNodeQueries(size_t *successes, size_t *failures);
bool testCharNodeQueries(size_t *successes, size_t *failures);
bool testArrayNodeQueries(size_t *successes, size_t *failures);

#endif
