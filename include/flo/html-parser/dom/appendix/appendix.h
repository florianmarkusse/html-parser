#ifndef FLO_HTML_PARSER_DOM_APPENDIX_APPENDIX_H
#define FLO_HTML_PARSER_DOM_APPENDIX_APPENDIX_H

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/document-node.h"
#include "flo/html-parser/type/node/node.h"

DomStatus appendDocumentNodeWithQuery(const char *cssQuery,
                                      const DocumentNode *docNode, Dom *dom,
                                      DataContainer *dataContainer);

DomStatus appendTextNodeWithQuery(const char *cssQuery, const char *text,
                                  Dom *dom, DataContainer *dataContainer);

DomStatus appendNodeFromStringWithQuery(const char *cssQuery,
                                        const char *htmlString, Dom *dom,
                                        DataContainer *dataContainer);

DomStatus appendNodeFromFileWithQuery(const char *cssQuery,
                                      const char *fileLocation, Dom *dom,
                                      DataContainer *dataContainer);

DomStatus appendDocumentNode(node_id parentID, const DocumentNode *docNode,
                             Dom *dom, DataContainer *dataContainer);
DomStatus appendTextNode(node_id parentID, const char *text, Dom *dom,
                         DataContainer *dataContainer);
DomStatus appendNodesFromString(node_id parentID, const char *htmlString,
                                Dom *dom, DataContainer *dataContainer);

#endif
