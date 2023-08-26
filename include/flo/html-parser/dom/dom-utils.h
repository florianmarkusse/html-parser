#ifndef FLO_HTML_PARSER_DOM_DOM_UTILS_H
#define FLO_HTML_PARSER_DOM_DOM_UTILS_H

#include "dom.h"

const char *getTag(indexID tagID, const Dom *dom,
                   const DataContainer *dataContainer);
void getTagRegistration(indexID tagID, const Dom *dom,
                        TagRegistration **tagRegistration);

const char *getBoolProp(indexID boolPropID, const Dom *dom,
                        const DataContainer *dataContainer);
const char *getPropKey(indexID propKeyID, const Dom *dom,
                       const DataContainer *dataContainer);
const char *getPropValue(indexID propValueID, const Dom *dom,
                         const DataContainer *dataContainer);
const char *getText(indexID textID, const Dom *dom,
                    const DataContainer *dataContainer);

node_id getFirstChild(node_id parentID, const Dom *dom);
node_id getNextNode(node_id currentNodeID, const Dom *dom);
node_id getParentNode(node_id currentNodeID, const Dom *dom);
node_id traverseDom(node_id currentNodeID, const Dom *dom);

#endif
