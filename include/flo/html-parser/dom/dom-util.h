#ifndef FLO_HTML_PARSER_DOM_DOM_UTIL_H
#define FLO_HTML_PARSER_DOM_DOM_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/dom/dom.h"

flo_html_node_id flo_html_createNode(flo_html_NodeType nodeType,
                                     flo_html_Dom *dom, flo_html_Arena *perm);
void flo_html_setNodeTagID(flo_html_node_id nodeID, flo_html_index_id tagID,
                           flo_html_Dom *dom);
void flo_html_setNodeText(flo_html_node_id nodeID, const flo_html_String text,
                          flo_html_Dom *dom);

const flo_html_String flo_html_getTag(flo_html_index_id tagID,
                                      flo_html_Dom *dom);

const flo_html_String flo_html_getBoolProp(flo_html_index_id boolPropID,
                                           flo_html_Dom *dom);
const flo_html_String flo_html_getPropKey(flo_html_index_id propKeyID,
                                          flo_html_Dom *dom);
const flo_html_String flo_html_getPropValue(flo_html_index_id propValueID,
                                            flo_html_Dom *dom);

bool flo_html_tryMerge(flo_html_Node *possibleMergeNode,
                       flo_html_Node *replacingNode, flo_html_Dom *dom,
                       bool isAppend, flo_html_Arena *perm);

void flo_html_connectOtherNodesToParent(flo_html_node_id parentID,
                                        flo_html_node_id lastAddedChild,
                                        flo_html_Dom *dom,
                                        flo_html_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
