#ifndef DOM_APPENDING_APPENDING_H
#define DOM_APPENDING_APPENDING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

#include "memory.h"

bool testflo_html_DomAppendices(ptrdiff_t *successes, ptrdiff_t *failures,
                                flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
