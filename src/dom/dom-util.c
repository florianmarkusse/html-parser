#include <stdbool.h>
#include <string.h>

#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/node/tag-registration.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/array.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

flo_html_node_id flo_html_createNode(const flo_html_NodeType nodeType,
                                     flo_html_Dom *dom, flo_html_Arena *perm) {
    flo_html_Node node;
    node.nodeType = nodeType;
    node.nodeID = dom->nodes.len;

    *FLO_HTML_PUSH(&dom->nodes, perm) = node;

    return node.nodeID;
}

void flo_html_setNodeTagID(const flo_html_node_id nodeID,
                           const flo_html_index_id tagID, flo_html_Dom *dom) {
    flo_html_Node *createdNode = &(dom->nodes.buf[nodeID]);
    createdNode->tagID = tagID;
}

void flo_html_setNodeText(const flo_html_node_id nodeID,
                          const flo_html_String text, flo_html_Dom *dom) {
    flo_html_Node *createdNode = &(dom->nodes.buf[nodeID]);
    createdNode->text = text;
}

const flo_html_String flo_html_getTag(const flo_html_index_id tagID,
                                      flo_html_ParsedHTML parsed) {
    flo_html_TagRegistration *tagRegistration =
        &parsed.dom->tagRegistry.buf[tagID];
    return flo_html_getStringFromHashSet(&parsed.textStore->tags,
                                         &tagRegistration->hashElement);
}

// TODO: get rid of these functions and use a single generic one
const flo_html_String flo_html_getBoolProp(const flo_html_index_id boolPropID,
                                           flo_html_ParsedHTML parsed) {
    return flo_html_getStringFromHashSet(
        &parsed.textStore->boolProps,
        &parsed.dom->boolPropRegistry.buf[boolPropID]);
}

// TODO: get rid of these functions and use a single generic one
const flo_html_String flo_html_getPropKey(const flo_html_index_id propKeyID,
                                          flo_html_ParsedHTML parsed) {
    return flo_html_getStringFromHashSet(
        &parsed.textStore->propKeys,
        &parsed.dom->propKeyRegistry.buf[propKeyID]);
}

// TODO: get rid of these functions and use a single generic one
const flo_html_String flo_html_getPropValue(const flo_html_index_id propValueID,
                                            flo_html_ParsedHTML parsed) {
    return flo_html_getStringFromHashSet(
        &parsed.textStore->propValues,
        &parsed.dom->propValueRegistry.buf[propValueID]);
}

bool flo_html_tryMerge(flo_html_Node *possibleMergeNode,
                       flo_html_Node *replacingNode, flo_html_ParsedHTML parsed,
                       bool isAppend, flo_html_Arena *perm) {
    if (possibleMergeNode->nodeType == NODE_TYPE_TEXT) {
        flo_html_addTextToTextNode(possibleMergeNode, replacingNode->text,
                                   parsed, isAppend, perm);
        return true;
    }
    return false;
}

void flo_html_connectOtherNodesToParent(const flo_html_node_id parentID,
                                        const flo_html_node_id lastAddedChild,
                                        flo_html_Dom *dom,
                                        flo_html_Arena *perm) {
    flo_html_node_id otherNewNodeID = flo_html_getNext(lastAddedChild, dom);
    while (otherNewNodeID > 0) {
        *FLO_HTML_PUSH(&dom->parentChilds, perm) = (flo_html_ParentChild){
            .parentID = parentID, .childID = otherNewNodeID};

        otherNewNodeID = flo_html_getNext(otherNewNodeID, dom);
    }
}
