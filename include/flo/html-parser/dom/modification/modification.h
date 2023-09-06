#ifndef FLO_HTML_PARSER_DOM_MODIFICATION_MODIFICATION_H
#define FLO_HTML_PARSER_DOM_MODIFICATION_MODIFICATION_H

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/element/element-status.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/type/node/node.h"

typedef enum {
    PROPERTY_TYPE_BOOL,
    PROPERTY_TYPE_KEY,
    PROPERTY_TYPE_VALUE,
    NUM_PROPERTY_TYPES
} PropertyType;

ElementStatus addPropertyToNodeStringsWithLength(node_id nodeID,
                                                 const char *keyBuffer,
                                                 size_t keyLen,
                                                 const char *valueBuffer,
                                                 size_t valueLen, Dom *dom,
                                                 DataContainer *dataContainer);
ElementStatus addPropertyToNodeStrings(node_id nodeID, const char *keyBuffer,
                                       const char *valueBuffer, Dom *dom,
                                       DataContainer *dataContainer);

ElementStatus addBooleanPropertyToNodeStringWithLength(
    node_id nodeID, const char *boolPropBuffer, size_t boolPropLen, Dom *dom,
    DataContainer *dataContainer);
ElementStatus addBooleanPropertyToNodeString(node_id nodeID,
                                             const char *boolPropBuffer,
                                             Dom *dom,
                                             DataContainer *dataContainer);

ElementStatus setPropertyValue(node_id nodeID, const char *key,
                               const char *newValue, Dom *dom,
                               DataContainer *dataContainer);

ElementStatus appendTextToTextNode(Node *node, const char *textStart,
                                   size_t textLen, Dom *dom,
                                   DataContainer *dataContainer);

DomStatus setTagOnDocumentNode(const char *tagStart, size_t elementStart,
                               node_id nodeID, bool isPaired, Dom *dom,
                               DataContainer *dataContainer);

#endif
