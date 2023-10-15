#ifndef DOM_PREPENDING_PREPENDING_H
#define DOM_PREPENDING_PREPENDING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <flo/html-parser/user.h>
#include <stdbool.h>
#include <stddef.h>

bool testflo_html_DomPrependices(ptrdiff_t *successes, ptrdiff_t *failures,
                                 flo_html_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
