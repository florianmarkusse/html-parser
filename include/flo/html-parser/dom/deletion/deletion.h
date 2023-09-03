#ifndef FLO_HTML_PARSER_DOM_DELETION_DELETION_H
#define FLO_HTML_PARSER_DOM_DELETION_DELETION_H

#include "flo/html-parser/dom/dom.h"

void removeNode(node_id nodeID, Dom *dom);
void removeChildren(node_id nodeID, Dom *dom);

void removeBooleanProperty(node_id nodeID, const char *boolProp, Dom *dom,
                           const DataContainer *dataContainer);
void removeProperty(node_id nodeID, const char *keyProp, Dom *dom,
                    const DataContainer *dataContainer);
#endif
