#ifndef FLO_HTML_PARSER_DOM_DOM_UTIL_H
#define FLO_HTML_PARSER_DOM_DOM_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "flo/html-parser/dom/dom.h"

flo_html_node_id flo_html_createNode(flo_html_NodeType nodeType,
                                     flo_html_Dom *dom, flo_Arena *perm);

bool flo_html_tryMerge(flo_html_node_id possibleMergeNodeID,
                       flo_html_node_id replacingNodeID, flo_html_Dom *dom,
                       bool isAppend, flo_Arena *perm);

void flo_html_connectOtherNodesToParent(flo_html_node_id parentID,
                                        flo_html_node_id lastAddedChild,
                                        flo_html_Dom *dom,
                                        flo_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
