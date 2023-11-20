#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "error.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/traversal.h"

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
    ptrdiff_t filtersLen, flo_html_Dom *dom, flo_msi_Uint16 *set,
    flo_Arena *perm) {
    for (ptrdiff_t i = 0; i < dom->nodes.len; i++) {
        if (flo_html_filterNode(dom->nodes.buf[i].nodeID, filters, filtersLen,
                                dom)) {
            flo_msi_html_uint16Insert(dom->nodes.buf[i].nodeID, set, perm);
        }
    }

    return true;
}

// TODO: only the end result is required on the perm arena.
flo_html_QueryStatus flo_html_getFilteredAdjacents(
    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    ptrdiff_t filtersLen, flo_html_Dom *dom, ptrdiff_t numberOfSiblings,
    flo_msi_Uint16 *set, flo_Arena *perm) {
    flo_msi_Uint16 filteredAdjacents =
        FLO_NEW_MSI_SET(flo_msi_Uint16, FLO_MSI_HTML_STARTING_EXPONENT, perm);

    uint16_t inSet;
    FLO_FOR_EACH_MSI_UINT16(inSet, set) {
        flo_html_node_id nextNodeID = flo_html_getNext(inSet, dom);
        ptrdiff_t siblingsNumberCopy = numberOfSiblings;

        while (siblingsNumberCopy > 0 && nextNodeID > 0) {
            if (flo_html_filterNode(nextNodeID, filters, filtersLen, dom)) {
                flo_msi_html_uint16Insert(nextNodeID, &filteredAdjacents, perm);
            }

            siblingsNumberCopy--;
            nextNodeID = flo_html_getNext(nextNodeID, dom);
        }
    }

    set->exp = filteredAdjacents.exp;
    set->len = filteredAdjacents.len;
    set->buf = filteredAdjacents.buf;

    return QUERY_SUCCESS;
}

// TODO: only the end result is required on the perm arena.
flo_html_QueryStatus flo_html_getFilteredDescendants(
    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    ptrdiff_t filtersLen, flo_html_Dom *dom, ptrdiff_t depth,
    flo_msi_Uint16 *set, flo_Arena *perm) {
    flo_msi_Uint16 firstDescendants = flo_msi_html_copyUint16(set, perm);
    flo_msi_html_resetUint16(set);

    flo_msi_Uint16 secondDescendants =
        FLO_NEW_MSI_SET(flo_msi_Uint16, firstDescendants.exp, perm);

    bool isFirstFilled = true;
    flo_msi_Uint16 *toBeFilledSet = &secondDescendants;
    flo_msi_Uint16 *filledSet = &firstDescendants;

    while (depth > 0 && filledSet->len > 0) {
        for (ptrdiff_t i = 0; i < dom->parentChilds.len; i++) {
            flo_html_ParentChild parentChildNode = dom->parentChilds.buf[i];
            if (dom->nodes.buf[parentChildNode.childID].nodeType ==
                    NODE_TYPE_DOCUMENT &&
                flo_msi_containsUint16(parentChildNode.parentID,
                                       flo_hash16_xm3(parentChildNode.parentID),
                                       filledSet)) {
                flo_msi_html_uint16Insert(parentChildNode.childID,
                                          toBeFilledSet, perm);

                if (flo_html_filterNode(parentChildNode.childID, filters,
                                        filtersLen, dom)) {
                    flo_msi_html_uint16Insert(parentChildNode.childID, set,
                                              perm);
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

        flo_msi_html_resetUint16(toBeFilledSet);

        depth--;
    }

    return QUERY_SUCCESS;
}
