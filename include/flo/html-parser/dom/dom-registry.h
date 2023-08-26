#ifndef FLO_HTML_PARSER_DOM_DOM_REGISTRY_H
#define FLO_HTML_PARSER_DOM_DOM_REGISTRY_H

#include "dom.h"

DomStatus addTagRegistration(indexID tagID, bool isPaired,
                             const HashElement *hashElement, Dom *dom);

DomStatus addBoolPropRegistration(indexID boolPropID,
                                  const HashElement *hashElement, Dom *dom);

DomStatus addPropKeyRegistration(indexID propKeyID,
                                 const HashElement *hashElement, Dom *dom);

DomStatus addPropValueRegistration(indexID propValueID,
                                   const HashElement *hashElement, Dom *dom);

DomStatus addTextRegistration(indexID textID, const HashElement *hashElement,
                              Dom *dom);

#endif
