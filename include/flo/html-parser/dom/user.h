#ifndef FLO_HTML_PARSER_DOM_USER_H
#define FLO_HTML_PARSER_DOM_USER_H

#include "dom.h"

DomStatus createFromFile(const char *fileLocation, Dom *dom,
                         DataContainer *dataContainer);

#endif
