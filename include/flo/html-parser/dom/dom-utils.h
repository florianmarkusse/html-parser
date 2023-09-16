#ifndef FLO_HTML_PARSER_DOM_DOM_UTILS_H
#define FLO_HTML_PARSER_DOM_DOM_UTILS_H

#include "flo/html-parser/dom/dom.h"

flo_html_DomStatus flo_html_createNode(flo_html_node_id *nodeID,
                                       flo_html_NodeType nodeType,
                                       flo_html_Dom *dom);
void flo_html_setNodeTagID(flo_html_node_id nodeID, flo_html_indexID tagID,
                           flo_html_Dom *dom);
void flo_html_setNodeText(flo_html_node_id nodeID, const char *text,
                          flo_html_Dom *dom);

flo_html_DomStatus flo_html_addParentFirstChild(flo_html_node_id parentID,
                                                flo_html_node_id childID,
                                                flo_html_Dom *dom);

flo_html_DomStatus flo_html_addParentChild(flo_html_node_id parentID,
                                           flo_html_node_id childID,
                                           flo_html_Dom *dom);

flo_html_DomStatus flo_html_addNextNode(flo_html_node_id currentNodeID,
                                        flo_html_node_id nextNodeID,
                                        flo_html_Dom *dom);

flo_html_DomStatus flo_html_addBooleanProperty(flo_html_node_id nodeID,
                                               flo_html_element_id propID,
                                               flo_html_Dom *dom);

flo_html_DomStatus flo_html_addProperty(flo_html_node_id nodeID,
                                        flo_html_element_id keyID,
                                        flo_html_element_id valueID,
                                        flo_html_Dom *dom);

const char *flo_html_getTag(flo_html_indexID tagID, const flo_html_Dom *dom,
                            const flo_html_TextStore *textStore);
void flo_html_getTagRegistration(flo_html_indexID tagID,
                                 const flo_html_Dom *dom,
                                 flo_html_TagRegistration **tagRegistration);

const char *flo_html_getBoolProp(flo_html_indexID boolPropID,
                                 const flo_html_Dom *dom,
                                 const flo_html_TextStore *textStore);
const char *flo_html_getPropKey(flo_html_indexID propKeyID,
                                const flo_html_Dom *dom,
                                const flo_html_TextStore *textStore);
const char *flo_html_getPropValue(flo_html_indexID propValueID,
                                  const flo_html_Dom *dom,
                                  const flo_html_TextStore *textStore);

typedef enum { COMPLETED_MERGE, NO_MERGE, FAILED_MERGE } flo_html_MergeResult;

flo_html_MergeResult flo_html_tryMerge(flo_html_Node *possibleMergeNode,
                                       flo_html_Node *replacingNode,
                                       flo_html_Dom *dom,
                                       flo_html_TextStore *textStore,
                                       bool isAppend);

flo_html_DomStatus
flo_html_connectOtherNodesToParent(flo_html_node_id parentID,
                                   flo_html_node_id lastAddedChild,
                                   flo_html_Dom *dom);

#endif
