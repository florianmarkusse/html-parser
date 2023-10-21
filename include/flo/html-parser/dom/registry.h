#ifndef FLO_HTML_PARSER_DOM_REGISTRY_H
#define FLO_HTML_PARSER_DOM_REGISTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dom.h"
#include "flo/html-parser/util/memory.h"

void flo_html_addRegistration(flo_html_HashElement hashElement,
                              flo_html_HashElement_d_a *hashElements,
                              flo_html_Arena *perm);
void flo_html_addTagRegistration(bool isPaired,
                                 flo_html_HashElement hashElement,
                                 flo_html_Dom *dom, flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
