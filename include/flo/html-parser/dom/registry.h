#ifndef FLO_HTML_PARSER_DOM_REGISTRY_H
#define FLO_HTML_PARSER_DOM_REGISTRY_H

#include "dom.h"

DomStatus addRegistration(indexID indexID, const HashElement *hashElement,
                          BasicRegistry *basicRegistry);
DomStatus addTagRegistration(indexID tagID, bool isPaired,
                             const HashElement *hashElement, Dom *dom);

DomStatus addBoolPropRegistration(indexID boolPropID,
                                  const HashElement *hashElement, Dom *dom);

DomStatus addPropKeyRegistration(indexID propKeyID,
                                 const HashElement *hashElement, Dom *dom);

DomStatus addPropValueRegistration(indexID propValueID,
                                   const HashElement *hashElement, Dom *dom);

#endif