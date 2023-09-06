#ifndef FLO_HTML_PARSER_DOM_REPLACEMENT_REPLACEMENT_H
#define FLO_HTML_PARSER_DOM_REPLACEMENT_REPLACEMENT_H

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/document-node.h"
#include "flo/html-parser/type/node/node.h"

DomStatus replaceWithDocumentNode(node_id toReplaceNodeID,
                                  const DocumentNode *docNode, Dom *dom,
                                  DataContainer *dataContainer);
DomStatus replaceWithTextNode(node_id toReplaceNodeID, const char *text,
                              Dom *dom, DataContainer *dataContainer);
DomStatus replaceWithNodesFromString(node_id toReplaceNodeID,
                                     const char *htmlString, Dom *dom,
                                     DataContainer *dataContainer);

#endif
