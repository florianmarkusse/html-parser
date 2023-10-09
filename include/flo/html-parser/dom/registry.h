#ifndef FLO_HTML_PARSER_DOM_REGISTRY_H
#define FLO_HTML_PARSER_DOM_REGISTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dom.h"

void flo_html_addRegistration(const flo_html_HashElement *hashElement,
                              flo_html_BasicRegistry *basicRegistry);
void flo_html_addTagRegistration(flo_html_index_id tagID, bool isPaired,
                                 const flo_html_HashElement *hashElement,
                                 flo_html_Dom *dom);

#ifdef __cplusplus
}
#endif

#endif
