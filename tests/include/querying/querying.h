#ifndef QUERYING_QUERYING_H
#define QUERYING_QUERYING_H

#include <stddef.h>

unsigned char testQueries(size_t *successes, size_t *failures);
unsigned char testQuerySelectorAll(size_t *successes, size_t *failures);
unsigned char testQuerySelector(size_t *successes, size_t *failures);

#endif
