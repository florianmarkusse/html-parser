#ifndef FLO_HTML_PARSER_DOM_REGISTRY_H
#define FLO_HTML_PARSER_DOM_REGISTRY_H

#ifdef __cplusplus
extern "C" {
#endif

#include "dom.h"

flo_html_DomStatus
flo_html_addRegistration(const flo_html_HashElement *hashElement,
                         flo_html_BasicRegistry *basicRegistry);
flo_html_DomStatus
flo_html_addTagRegistration(flo_html_indexID tagID, bool isPaired,
                            const flo_html_HashElement *hashElement,
                            flo_html_Dom *dom);

flo_html_DomStatus
flo_html_addBoolPropRegistration(const flo_html_HashElement *hashElement,
                                 flo_html_Dom *dom);

flo_html_DomStatus
flo_html_addPropKeyRegistration(const flo_html_HashElement *hashElement,
                                flo_html_Dom *dom);

flo_html_DomStatus
flo_html_addPropValueRegistration(const flo_html_HashElement *hashElement,
                                  flo_html_Dom *dom);

#ifdef __cplusplus
}
#endif

#endif
