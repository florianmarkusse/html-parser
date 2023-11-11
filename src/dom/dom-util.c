#include <stdbool.h>
#include <string.h>

#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/node/tag-registration.h"
#include "flo/html-parser/parser.h"
#include "array.h"
#include "error.h"
#include "memory.h"

flo_html_node_id flo_html_createNode(flo_html_NodeType nodeType,
                                     flo_html_Dom *dom, flo_Arena *perm) {
    if (dom->nodes.len > FLO_HTML_MAX_NODE_ID - 1) {
        __builtin_longjmp(perm->jmp_buf, 1);
    }
    flo_html_Node node;
    node.nodeType = nodeType;
    node.nodeID = (flo_html_node_id)dom->nodes.len;

    *FLO_PUSH(&dom->nodes, perm) = node;

    return node.nodeID;
}

bool flo_html_tryMerge(flo_html_node_id possibleMergeNodeID,
                       flo_html_node_id replacingNodeID, flo_html_Dom *dom,
                       bool isAppend, flo_Arena *perm) {
    if (dom->nodes.buf[possibleMergeNodeID].nodeType == NODE_TYPE_TEXT) {
        flo_html_addTextToTextNode(possibleMergeNodeID,
                                   dom->nodes.buf[replacingNodeID].text, dom,
                                   isAppend, perm);
        return true;
    }
    return false;
}

void flo_html_connectOtherNodesToParent(flo_html_node_id parentID,
                                        flo_html_node_id lastAddedChild,
                                        flo_html_Dom *dom,
                                        flo_Arena *perm) {
    flo_html_node_id otherNewNodeID = flo_html_getNext(lastAddedChild, dom);
    while (otherNewNodeID > 0) {
        *FLO_PUSH(&dom->parentChilds, perm) = (flo_html_ParentChild){
            .parentID = parentID, .childID = otherNewNodeID};

        otherNewNodeID = flo_html_getNext(otherNewNodeID, dom);
    }
}
