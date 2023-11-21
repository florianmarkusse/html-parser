#ifndef DOM_DUPLICATING_DUPLICATING_H
#define DOM_DUPLICATING_DUPLICATING_H

#ifdef __cplusplus
extern "C" {
#endif

#include "memory/arena.h"
#include <stddef.h>

bool testflo_html_DomDuplications(ptrdiff_t *successes, ptrdiff_t *failures,
                                  flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
