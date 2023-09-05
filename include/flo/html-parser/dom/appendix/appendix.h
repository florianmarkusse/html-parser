#ifndef FLO_HTML_PARSER_DOM_APPENDIX_APPENDIX_H
#define FLO_HTML_PARSER_DOM_APPENDIX_APPENDIX_H

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/document-node.h"
#include "flo/html-parser/type/node/node.h"

DomStatus appendDocumentNode(node_id parentID, const DocumentNode *docNode,
                             Dom *dom, DataContainer *dataContainer);
DomStatus appendTextNode(node_id parentID, const char *text, Dom *dom,
                         DataContainer *dataContainer);
DomStatus appendNodesFromString(node_id parentID, const char *htmlString,
                                Dom *dom, DataContainer *dataContainer);

#endif
