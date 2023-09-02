#ifndef FLO_HTML_PARSER_DOM_READING_READING_UTIL_H
#define FLO_HTML_PARSER_DOM_READING_READING_UTIL_H

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/type/node/property.h"

Property *getProperty(node_id nodeID, element_id propKeyID, const Dom *dom);
#endif
