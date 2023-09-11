#ifndef FLO_HTML_PARSER_DOM_DOM_UTILS_H
#define FLO_HTML_PARSER_DOM_DOM_UTILS_H

#include "flo/html-parser/dom/dom.h"

DomStatus createNode(node_id *nodeID, NodeType nodeType, Dom *dom);
void setNodeTagID(node_id nodeID, indexID tagID, Dom *dom);
void setNodeText(node_id nodeID, const char *text, Dom *dom);

DomStatus addParentFirstChild(node_id parentID, node_id childID, Dom *dom);

DomStatus addParentChild(node_id parentID, node_id childID, Dom *dom);

DomStatus addNextNode(node_id currentNodeID, node_id nextNodeID, Dom *dom);

DomStatus addBooleanProperty(node_id nodeID, element_id propID, Dom *dom);

DomStatus addProperty(node_id nodeID, element_id keyID, element_id valueID,
                      Dom *dom);

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
