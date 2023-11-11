#ifndef INTEGRATION_TEST_H
#define INTEGRATION_TEST_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

bool testIntegrations(ptrdiff_t *successes, ptrdiff_t *failures,
                      flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
