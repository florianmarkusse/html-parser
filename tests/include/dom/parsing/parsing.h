#ifndef DOM_PARSING_PARSING_H
#define DOM_PARSING_PARSING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>

bool testflo_html_DomParsings(ptrdiff_t *successes, ptrdiff_t *failures,
                              flo_html_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
