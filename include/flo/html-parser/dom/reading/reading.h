#ifndef FLO_HTML_PARSER_DOM_READING_READING_H
#define FLO_HTML_PARSER_DOM_READING_READING_H

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-status.h"
#include "flo/html-parser/type/node/node.h"

Node getNode(node_id nodeID, const Dom *dom);

NodeType getNodeType(node_id nodeID, const Dom *dom);

bool hasBoolProp(node_id nodeID, const char *boolProp, const Dom *dom,
                 const DataContainer *dataContainer);
bool hasPropKey(node_id nodeID, const char *propKey, const Dom *dom,
                const DataContainer *dataContainer);
bool hasPropValue(node_id nodeID, const char *propValue, const Dom *dom,
                  const DataContainer *dataContainer);
bool hasProperty(node_id nodeID, const char *propKey, const char *propValue,
                 const Dom *dom, const DataContainer *dataContainer);
const char *getValue(node_id nodeID, const char *propKey, const Dom *dom,
                     const DataContainer *dataContainer);
QueryStatus getTextContent(node_id nodeID, const Dom *dom,
                           const char ***results, size_t *reusultsLen);

#endif
