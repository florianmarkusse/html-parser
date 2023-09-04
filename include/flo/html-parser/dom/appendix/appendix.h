#ifndef FLO_HTML_PARSER_DOM_APPENDIX_APPENDIX_H
#define FLO_HTML_PARSER_DOM_APPENDIX_APPENDIX_H

#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/dom/dom-status.h"
#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/type/node/node.h"

#define MAX_PROPS_LEN_APPENDIX (1U << 4U)

typedef struct {
    const char *tag;
    const bool isPaired;
    const char *boolProps[MAX_PROPS_LEN_APPENDIX];
    const size_t boolPropsLen;
    const char *keyProps[MAX_PROPS_LEN_APPENDIX];
    const char *valueProps[MAX_PROPS_LEN_APPENDIX];
    const size_t propsLen;
} __attribute__((aligned(128))) AppendDocumentNode;

DomStatus addTagToNodeID(const char *tagStart, size_t elementStart,
                         node_id nodeID, bool isPaired, Dom *dom,
                         DataContainer *dataContainer);
DomStatus appendDocumentNode(node_id parentID,
                             const AppendDocumentNode *docNode, Dom *dom,
                             DataContainer *dataContainer);
DomStatus appendTextNode(node_id parentID, const char *text, Dom *dom,
                         DataContainer *dataContainer);

#endif
