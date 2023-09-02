#ifndef FLO_HTML_PARSER_DOM_UTILS_H
#define FLO_HTML_PARSER_DOM_UTILS_H

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

#endif