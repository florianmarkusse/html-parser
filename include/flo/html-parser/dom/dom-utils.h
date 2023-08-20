#ifndef FLO_HTML_PARSER_DOM_DOM_UTILS_H
#define FLO_HTML_PARSER_DOM_DOM_UTILS_H

#include "dom.h"

const char *getText(node_id nodeID, const Dom *dom,
                    const DataContainer *dataContainer);
node_id getFirstChild(node_id parentID, const Dom *dom);
node_id getNextNode(node_id currentNodeID, const Dom *dom);
node_id getParentNode(node_id currentNodeID, const Dom *dom);
node_id traverseDom(node_id currentNodeID, const Dom *dom);

#endif
