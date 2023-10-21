#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/util/error.h"

bool flo_html_filterNode(const flo_html_node_id nodeID,
                         const flo_html_FilterType *filters,
                         const ptrdiff_t filterslen, const flo_html_Dom *dom) {
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

// flo_html_containsStringWithDataHashSet(set, string);

// TODO: only the end result is required on the perm arena.
bool flo_html_getNodesWithoutflo_html_Combinator(
    const flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    const ptrdiff_t filtersLen, const flo_html_Dom *dom,
    flo_html_Uint16HashSet *set, flo_html_Arena *perm) {
    for (ptrdiff_t i = 0; i < dom->nodes.len; i++) {
        if (flo_html_filterNode(dom->nodes.buf[i].nodeID, filters, filtersLen,
                                dom)) {
            if (!flo_html_insertUint16HashSet(set, dom->nodes.buf[i].nodeID,
                                              perm)) {
                return false;
            }
        }
    }

    return true;
}

// TODO(florian): not very nice way of doing this. Use a
// hash or something.
bool isPresentIn(const flo_html_node_id nodeID, const flo_html_node_id *array,
                 const ptrdiff_t arrayLen) {
    for (ptrdiff_t j = 0; j < arrayLen; j++) {
        if (nodeID == array[j]) {
            return true;
        }
    }

    return false;
}

// TODO: only the end result is required on the perm arena.
flo_html_QueryStatus flo_html_getFilteredAdjacents(
    const flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    const ptrdiff_t filtersLen, const flo_html_Dom *dom,
    const ptrdiff_t numberOfSiblings, flo_html_Uint16HashSet *set,
    flo_html_Arena *perm) {
    flo_html_Uint16HashSet filteredAdjacents =
        flo_html_initUint16HashSet(set->arrayLen, perm);
    flo_html_Uint16HashSetIterator iterator =
        flo_html_initUint16HashSetIterator(set);

    while (flo_html_hasNextUint16HashSetIterator(&iterator)) {
        flo_html_node_id inSet = flo_html_nextUint16HashSetIterator(&iterator);
        flo_html_node_id nextNodeID = flo_html_getNext(inSet, dom);
        ptrdiff_t siblingsNumberCopy = numberOfSiblings;

        while (siblingsNumberCopy > 0 && nextNodeID > 0) {
            if (flo_html_filterNode(nextNodeID, filters, filtersLen, dom)) {
                if (!flo_html_insertUint16HashSet(&filteredAdjacents,
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
    const flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    const ptrdiff_t filtersLen, const flo_html_Dom *dom, ptrdiff_t depth,
    flo_html_Uint16HashSet *set, flo_html_Arena *perm) {
    flo_html_Uint16HashSet firstDescendants =
        flo_html_copyUint16HashSet(set, perm);
    flo_html_resetUint16HashSet(set);

    flo_html_Uint16HashSet secondDescendants =
        flo_html_initUint16HashSet(firstDescendants.arrayLen, perm);

    bool isFirstFilled = true;
    flo_html_Uint16HashSet *toBeFilledSet = &secondDescendants;
    flo_html_Uint16HashSet *filledSet = &firstDescendants;

    while (depth > 0 && filledSet->entries > 0) {
        for (ptrdiff_t i = 0; i < dom->parentChilds.len; i++) {
            flo_html_ParentChild parentChildNode = dom->parentChilds.buf[i];
            if (dom->nodes.buf[parentChildNode.childID].nodeType ==
                    NODE_TYPE_DOCUMENT &&
                flo_html_containsUint16HashSet(filledSet,
                                               parentChildNode.parentID)) {
                if (!flo_html_insertUint16HashSet(
                        toBeFilledSet, parentChildNode.childID, perm)) {
                    FLO_HTML_PRINT_ERROR("inserting into hash set failed!\n");
                    return QUERY_MEMORY_ERROR;
                }
                if (flo_html_filterNode(parentChildNode.childID, filters,
                                        filtersLen, dom)) {
                    if (!flo_html_insertUint16HashSet(
                            set, parentChildNode.childID, perm)) {
                        FLO_HTML_PRINT_ERROR(
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

        flo_html_resetUint16HashSet(toBeFilledSet);

        depth--;
    }

    return QUERY_SUCCESS;
}
