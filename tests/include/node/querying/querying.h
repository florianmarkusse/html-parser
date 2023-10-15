#ifndef NODE_QUERYING_QUERYING_H
#define NODE_QUERYING_QUERYING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <flo/html-parser/user.h>
#include <stdbool.h>
#include <stddef.h>

bool testNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                     flo_html_Arena scratch);
bool testBoolNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                         flo_html_Arena scratch);
bool testCharNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                         flo_html_Arena scratch);
bool testArrayNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                          flo_html_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
