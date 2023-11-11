#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "error.h"

bool flo_html_filterNode(flo_html_node_id nodeID, flo_html_FilterType *filters,
                         ptrdiff_t filterslen, flo_html_Dom *dom) {
    for (ptrdiff_t i = 0; i < filterslen; i++) {
        flo_html_FilterType filterType = filters[i];
        switch (filterType.attributeSelector) {
        case ALL_NODES: {
            return true;
            break;
        }
        case TAG: {
            if (dom->nodes.buf[nodeID].tagID != filterType.data.tagID) {
                return false;
            }
            break;
        }
        case BOOLEAN_PROPERTY: {
            bool flo_html_hasBoolProp = false;
            // TODO(florian): find way to improve this.
            for (ptrdiff_t j = 0; j < dom->boolProps.len; j++) {
                if (dom->boolProps.buf[j].nodeID == nodeID &&
                    dom->boolProps.buf[j].propID == filterType.data.propID) {
                    flo_html_hasBoolProp = true;
                    break;
                }
            }
            if (!flo_html_hasBoolProp) {
                return false;
            }
            break;
        }
        case PROPERTY: {
            bool hasProp = false;
            // TODO(florian): find way to improve this.
            for (ptrdiff_t j = 0; j < dom->props.len; j++) {
                if (dom->props.buf[j].nodeID == nodeID &&
                    dom->props.buf[j].keyID ==
                        filterType.data.keyValuePair.keyID &&
                    dom->props.buf[j].valueID ==
                        filterType.data.keyValuePair.valueID) {
                    hasProp = true;
                    break;
                }
            }
            if (!hasProp) {
                return false;
            }
            break;
        }
        }
    }

    return true;
}

bool flo_html_getNodesWithoutflo_html_Combinator(
    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    ptrdiff_t filtersLen, flo_html_Dom *dom, flo_Uint16HashSet *set,
    flo_Arena *perm) {
    for (ptrdiff_t i = 0; i < dom->nodes.len; i++) {
        if (flo_html_filterNode(dom->nodes.buf[i].nodeID, filters, filtersLen,
                                dom)) {
            if (!flo_insertUint16HashSet(set, dom->nodes.buf[i].nodeID,
                                              perm)) {
                return false;
            }
        }
    }

    return true;
}

// TODO: only the end result is required on the perm arena.
flo_html_QueryStatus flo_html_getFilteredAdjacents(
    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    ptrdiff_t filtersLen, flo_html_Dom *dom, ptrdiff_t numberOfSiblings,
    flo_Uint16HashSet *set, flo_Arena *perm) {
    flo_Uint16HashSet filteredAdjacents =
        flo_initUint16HashSet((flo_html_node_id)set->arrayLen, perm);
    flo_Uint16HashSetIterator iterator =
        (flo_Uint16HashSetIterator){.set = set, .index = 0};

    flo_html_node_id inSet;
    while ((inSet = flo_nextUint16HashSetIterator(&iterator)) != 0) {
        flo_html_node_id nextNodeID = flo_html_getNext(inSet, dom);
        ptrdiff_t siblingsNumberCopy = numberOfSiblings;

        while (siblingsNumberCopy > 0 && nextNodeID > 0) {
            if (flo_html_filterNode(nextNodeID, filters, filtersLen, dom)) {
                if (!flo_insertUint16HashSet(&filteredAdjacents,
                                                  nextNodeID, perm)) {
                    return QUERY_MEMORY_ERROR;
                }
            }

            siblingsNumberCopy--;
            nextNodeID = flo_html_getNext(nextNodeID, dom);
        }
    }

    set->arrayLen = filteredAdjacents.arrayLen;
    set->array = filteredAdjacents.array;
    set->entries = filteredAdjacents.entries;

    return QUERY_SUCCESS;
}

// TODO: only the end result is required on the perm arena.
flo_html_QueryStatus flo_html_getFilteredDescendants(
    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    ptrdiff_t filtersLen, flo_html_Dom *dom, ptrdiff_t depth,
    flo_Uint16HashSet *set, flo_Arena *perm) {
    flo_Uint16HashSet firstDescendants =
        flo_copyUint16HashSet(set, perm);
    flo_resetUint16HashSet(set);

    flo_Uint16HashSet secondDescendants = flo_initUint16HashSet(
        (flo_html_node_id)firstDescendants.arrayLen, perm);

    bool isFirstFilled = true;
    flo_Uint16HashSet *toBeFilledSet = &secondDescendants;
    flo_Uint16HashSet *filledSet = &firstDescendants;

    while (depth > 0 && filledSet->entries > 0) {
        for (ptrdiff_t i = 0; i < dom->parentChilds.len; i++) {
            flo_html_ParentChild parentChildNode = dom->parentChilds.buf[i];
            if (dom->nodes.buf[parentChildNode.childID].nodeType ==
                    NODE_TYPE_DOCUMENT &&
                flo_containsUint16HashSet(filledSet,
                                               parentChildNode.parentID)) {
                if (!flo_insertUint16HashSet(
                        toBeFilledSet, parentChildNode.childID, perm)) {
                    FLO_PRINT_ERROR("inserting into hash set failed!\n");
                    return QUERY_MEMORY_ERROR;
                }
                if (flo_html_filterNode(parentChildNode.childID, filters,
                                        filtersLen, dom)) {
                    if (!flo_insertUint16HashSet(
                            set, parentChildNode.childID, perm)) {
                        FLO_PRINT_ERROR(
                            "inserting into results set failed!\n");
                        return QUERY_MEMORY_ERROR;
                    }
                }
            }
        }

        isFirstFilled = !isFirstFilled;

        if (isFirstFilled) {
            toBeFilledSet = &secondDescendants;
            filledSet = &firstDescendants;
        } else {
            toBeFilledSet = &firstDescendants;
            filledSet = &secondDescendants;
        }

        flo_resetUint16HashSet(toBeFilledSet);

        depth--;
    }

    return QUERY_SUCCESS;
}
