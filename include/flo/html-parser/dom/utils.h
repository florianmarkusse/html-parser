#ifndef FLO_HTML_PARSER_DOM_UTILS_H
#define FLO_HTML_PARSER_DOM_UTILS_H

#include "dom.h"
#include "flo/html-parser/dom/dom-status.h"

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

typedef enum { COMPLETED_MERGE, NO_MERGE, FAILED_MERGE } MergeResult;

MergeResult tryMerge(Node *possibleMergeNode, Node *replacingNode, Dom *dom,
                     DataContainer *dataContainer, bool isAppend);

DomStatus connectOtherNodesToParent(node_id parentID, node_id lastAddedChild,
                                    Dom *dom);

#endif
