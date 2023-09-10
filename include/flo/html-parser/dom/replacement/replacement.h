#ifndef FLO_HTML_PARSER_DOM_REPLACEMENT_REPLACEMENT_H
#define FLO_HTML_PARSER_DOM_REPLACEMENT_REPLACEMENT_H

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/document-node.h"
#include "flo/html-parser/type/node/node.h"

DomStatus replaceWithDocumentNodeWithQuery(const char *cssQuery,
                                           const DocumentNode *docNode,
                                           Dom *dom,
                                           DataContainer *dataContainer);

DomStatus replaceWithTextNodeWithQuery(const char *cssQuery, const char *text,
                                       Dom *dom, DataContainer *dataContainer);
DomStatus replaceWithNodeFromStringWithQuery(const char *cssQuery,
                                             const char *htmlString, Dom *dom,
                                             DataContainer *dataContainer);

DomStatus replaceWithNodeFromFileWithQuery(const char *cssQuery,
                                           const char *fileLocation, Dom *dom,
                                           DataContainer *dataContainer);

DomStatus replaceWithDocumentNode(node_id toReplaceNodeID,
                                  const DocumentNode *docNode, Dom *dom,
                                  DataContainer *dataContainer);
DomStatus replaceWithTextNode(node_id toReplaceNodeID, const char *text,
                              Dom *dom, DataContainer *dataContainer);
DomStatus replaceWithNodesFromString(node_id toReplaceNodeID,
                                     const char *htmlString, Dom *dom,
                                     DataContainer *dataContainer);

#endif
