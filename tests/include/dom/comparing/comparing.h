#ifndef DOM_COMPARING_COMPARING_H
#define DOM_COMPARING_COMPARING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <flo/html-parser.h>
#include <stdbool.h>
#include <stddef.h>

bool testflo_html_DomComparisons(ptrdiff_t *successes, ptrdiff_t *failures,
                                 flo_html_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
