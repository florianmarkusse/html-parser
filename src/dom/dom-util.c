#include <stdbool.h>
#include <string.h>

#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/node/tag-registration.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

flo_html_node_id flo_html_createNode(const flo_html_NodeType nodeType,
                                     flo_html_Dom *dom) {
    // TODO: DYNAMIC
    if (dom->nodeLen >= dom->nodeCap) {
        FLO_HTML_PRINT_ERROR("Too many nodes created!\n");
    }

    flo_html_Node *newNode = &(dom->nodes[dom->nodeLen]);
    newNode->nodeType = nodeType;
    newNode->nodeID = dom->nodeLen;
    dom->nodeLen++;

    return newNode->nodeID;
}

void flo_html_setNodeTagID(const flo_html_node_id nodeID,
                           const flo_html_index_id tagID, flo_html_Dom *dom) {
    flo_html_Node *createdNode = &(dom->nodes[nodeID]);
    createdNode->tagID = tagID;
}

void flo_html_setNodeText(const flo_html_node_id nodeID,
                          const flo_html_String text, flo_html_Dom *dom) {
    flo_html_Node *createdNode = &(dom->nodes[nodeID]);
    createdNode->text = text;
}

void flo_html_addParentFirstChild(const flo_html_node_id parentID,
                                  const flo_html_node_id childID,
                                  flo_html_Dom *dom) {
    FLO_HTML_ASSERT(dom->parentFirstChildLen < dom->parentFirstChildCap);
    // TODO: DYNAMIC
    if (dom->parentFirstChildLen >= dom->parentFirstChildCap) {
        FLO_HTML_PRINT_ERROR("Too many parent first chillds created!\n");
    }

    flo_html_ParentChild *newParentFirstChild =
        &(dom->parentFirstChilds[dom->parentFirstChildLen]);
    newParentFirstChild->parentID = parentID;
    newParentFirstChild->childID = childID;
    dom->parentFirstChildLen++;
}

void flo_html_addParentChild(const flo_html_node_id parentID,
                             const flo_html_node_id childID,
                             flo_html_Dom *dom) {
    // TODO: DYNAMIC
    if (dom->parentChildLen >= dom->parentChildCap) {
        FLO_HTML_PRINT_ERROR("Too many parent chillds created!\n");
    }

    flo_html_ParentChild *newParentChild =
        &(dom->parentChilds[dom->parentChildLen]);
    newParentChild->parentID = parentID;
    newParentChild->childID = childID;
    dom->parentChildLen++;
}

void flo_html_addNextNode(const flo_html_node_id currentNodeID,
                          const flo_html_node_id nextNodeID,
                          flo_html_Dom *dom) {
    // TODO: DYNAMIC
    if (dom->nextNodeLen >= dom->nextNodeCap) {
        FLO_HTML_PRINT_ERROR("Too many parent chillds created!\n");
    }

    flo_html_NextNode *newNextNode = &(dom->nextNodes[dom->nextNodeLen]);
    newNextNode->currentNodeID = currentNodeID;
    newNextNode->nextNodeID = nextNodeID;
    dom->nextNodeLen++;
}

void flo_html_addBooleanProperty(const flo_html_node_id nodeID,
                                 const flo_html_index_id propID,
                                 flo_html_Dom *dom) {
    // TODO: DYNAMIC
    if (dom->boolPropsLen >= dom->boolPropsCap) {
        FLO_HTML_PRINT_ERROR("Too many parent chillds created!\n");
    }

    flo_html_BooleanProperty *newBooleanProperty =
        &(dom->boolProps[dom->boolPropsLen]);
    newBooleanProperty->nodeID = nodeID;
    newBooleanProperty->propID = propID;
    dom->boolPropsLen++;
}

void flo_html_addProperty(const flo_html_node_id nodeID,
                          const flo_html_index_id keyID,
                          const flo_html_index_id valueID, flo_html_Dom *dom) {
    // TODO: DYNAMIC
    if (dom->propsLen >= dom->propsCap) {
        FLO_HTML_PRINT_ERROR("Too many parent chillds created!\n");
    }

    flo_html_Property *newProperty = &(dom->props[dom->propsLen]);
    newProperty->nodeID = nodeID;
    newProperty->keyID = keyID;
    newProperty->valueID = valueID;
    dom->propsLen++;
}

const flo_html_String flo_html_getTag(const flo_html_index_id tagID,
                                      flo_html_ParsedHTML parsed) {
    flo_html_TagRegistration *tagRegistration = &parsed.dom->tagRegistry[tagID];
    return flo_html_getStringFromHashSet(&parsed.textStore->tags.set,
                                         &tagRegistration->hashElement);
}

// TODO: get rid of these functions and use a single generic one
const flo_html_String flo_html_getBoolProp(const flo_html_index_id boolPropID,
                                           flo_html_ParsedHTML parsed) {
    return flo_html_getStringFromHashSet(
        &parsed.textStore->boolProps.set,
        &parsed.dom->boolPropRegistry.hashes[boolPropID]);
}

// TODO: get rid of these functions and use a single generic one
const flo_html_String flo_html_getPropKey(const flo_html_index_id propKeyID,
                                          flo_html_ParsedHTML parsed) {
    return flo_html_getStringFromHashSet(
        &parsed.textStore->propKeys.set,
        &parsed.dom->propKeyRegistry.hashes[propKeyID]);
}

// TODO: get rid of these functions and use a single generic one
const flo_html_String flo_html_getPropValue(const flo_html_index_id propValueID,
                                            flo_html_ParsedHTML parsed) {
    return flo_html_getStringFromHashSet(
        &parsed.textStore->propValues.set,
        &parsed.dom->propValueRegistry.hashes[propValueID]);
}

bool flo_html_tryMerge(flo_html_Node *possibleMergeNode,
                       flo_html_Node *replacingNode, flo_html_ParsedHTML parsed,
                       bool isAppend) {
    if (possibleMergeNode->nodeType == NODE_TYPE_TEXT) {
        flo_html_addTextToTextNode(possibleMergeNode, replacingNode->text,
                                   parsed, isAppend);
        return true;
    }
    return false;
}

void flo_html_connectOtherNodesToParent(const flo_html_node_id parentID,
                                        const flo_html_node_id lastAddedChild,
                                        flo_html_Dom *dom) {
    flo_html_node_id otherNewNodeID = flo_html_getNext(lastAddedChild, dom);
    while (otherNewNodeID > 0) {
        flo_html_addParentChild(parentID, otherNewNodeID, dom);
        otherNewNodeID = flo_html_getNext(otherNewNodeID, dom);
    }
}
