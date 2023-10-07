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
            if (dom->nodes[nodeID].tagID != filterType.data.tagID) {
                return false;
            }
            break;
        }
        case BOOLEAN_PROPERTY: {
            bool flo_html_hasBoolProp = false;
            // TODO(florian): find way to improve this.
            for (ptrdiff_t j = 0; j < dom->boolPropsLen; j++) {
                if (dom->boolProps[j].nodeID == nodeID &&
                    dom->boolProps[j].propID == filterType.data.propID) {
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
            for (ptrdiff_t j = 0; j < dom->propsLen; j++) {
                if (dom->props[j].nodeID == nodeID &&
                    dom->props[j].keyID == filterType.data.keyValuePair.keyID &&
                    dom->props[j].valueID ==
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

flo_html_indexID flo_html_getTagID(const flo_html_String tag,
                                   const flo_html_TextStore *textStore) {
    flo_html_HashElement ignore;
    flo_html_indexID tagID = 0;
    flo_html_containsStringWithDataHashSet(&textStore->tags.set, tag, &ignore,
                                           &tagID);
    return tagID;
}

flo_html_indexID flo_html_getBoolPropID(const flo_html_String boolProp,
                                        const flo_html_TextStore *textStore) {
    flo_html_HashElement ignore;
    flo_html_indexID propID = 0;
    flo_html_containsStringWithDataHashSet(&textStore->boolProps.set, boolProp,
                                           &ignore, &propID);
    return propID;
}

flo_html_indexID flo_html_getPropKeyID(const flo_html_String keyProp,
                                       const flo_html_TextStore *textStore) {
    flo_html_HashElement ignore;
    flo_html_indexID keyID = 0;
    flo_html_containsStringWithDataHashSet(&textStore->propKeys.set, keyProp,
                                           &ignore, &keyID);
    return keyID;
}

flo_html_indexID flo_html_getPropValueID(const flo_html_String valueProp,
                                         const flo_html_TextStore *textStore) {
    flo_html_HashElement ignore;
    flo_html_indexID valueID = 0;
    flo_html_containsStringWithDataHashSet(&textStore->propValues.set,
                                           valueProp, &ignore, &valueID);
    return valueID;
}

flo_html_QueryStatus flo_html_getNodesWithoutflo_html_Combinator(
    const flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    const ptrdiff_t filtersLen, const flo_html_Dom *dom,
    flo_html_Uint16HashSet *set) {
    for (ptrdiff_t i = 0; i < dom->nodeLen; i++) {
        if (flo_html_filterNode(dom->nodes[i].nodeID, filters, filtersLen,
                                dom)) {
            flo_html_HashStatus status =
                flo_html_insertUint16HashSet(set, dom->nodes[i].nodeID);
            if (status != HASH_SUCCESS) {
                FLO_HTML_ERROR_WITH_CODE_ONLY(
                    flo_html_hashStatusToString(status),
                    "inserting into hash set failed!\n");
                return QUERY_MEMORY_ERROR;
            }
        }
    }

    return QUERY_SUCCESS;
}

flo_html_QueryStatus flo_html_filterByTagID(const flo_html_element_id tagID,
                                            const flo_html_Dom *dom,
                                            flo_html_node_id *results,
                                            ptrdiff_t *len) {
    ptrdiff_t nextFreeSpot = 0;
    for (ptrdiff_t i = 0; i < *len; i++) {
        if (dom->nodes[results[i]].tagID == tagID) {
            results[nextFreeSpot++] = results[i];
        }
    }

    *len = nextFreeSpot;
    return QUERY_SUCCESS;
}

// TODO(florian): not very nice way of doing this. Use a
// hash or something.
unsigned char isPresentIn(const flo_html_node_id nodeID,
                          const flo_html_node_id *array,
                          const ptrdiff_t arrayLen) {
    for (ptrdiff_t j = 0; j < arrayLen; j++) {
        if (nodeID == array[j]) {
            return 1;
        }
    }

    return 0;
}

flo_html_QueryStatus flo_html_getFilteredAdjacents(
    const flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    const ptrdiff_t filtersLen, const flo_html_Dom *dom,
    const ptrdiff_t numberOfSiblings, flo_html_Uint16HashSet *set) {
    flo_html_Uint16HashSet filteredAdjacents;
    if (flo_html_initUint16HashSet(&filteredAdjacents, set->arrayLen) !=
        HASH_SUCCESS) {
        FLO_HTML_PRINT_ERROR(
            "Could not allocate memory for the filtered adjacents set\n");
        return QUERY_MEMORY_ERROR;
    }

    flo_html_Uint16HashSetIterator iterator;
    flo_html_initUint16HashSetIterator(&iterator, set);

    while (flo_html_hasNextUint16HashSetIterator(&iterator)) {
        flo_html_node_id inSet = flo_html_nextUint16HashSetIterator(&iterator);
        flo_html_node_id nextNodeID = flo_html_getNext(inSet, dom);
        ptrdiff_t siblingsNumberCopy = numberOfSiblings;

        while (siblingsNumberCopy > 0 && nextNodeID > 0) {
            if (flo_html_filterNode(nextNodeID, filters, filtersLen, dom)) {
                flo_html_HashStatus status = flo_html_insertUint16HashSet(
                    &filteredAdjacents, nextNodeID);
                if (status != HASH_SUCCESS) {
                    flo_html_destroyUint16HashSet(&filteredAdjacents);
                    FLO_HTML_ERROR_WITH_CODE_ONLY(
                        flo_html_hashStatusToString(status),
                        "inserting into hash set failed!\n");
                    return QUERY_MEMORY_ERROR;
                }
            }

            siblingsNumberCopy--;
            nextNodeID = flo_html_getNext(nextNodeID, dom);
        }
    }

    flo_html_destroyUint16HashSet(set);
    set->arrayLen = filteredAdjacents.arrayLen;
    set->array = filteredAdjacents.array;
    set->entries = filteredAdjacents.entries;

    return QUERY_SUCCESS;
}

flo_html_QueryStatus flo_html_getFilteredDescendants(
    const flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT],
    const ptrdiff_t filtersLen, const flo_html_Dom *dom, ptrdiff_t depth,
    flo_html_Uint16HashSet *set) {
    flo_html_Uint16HashSet firstDescendants;
    if (flo_html_copyUint16HashSet(set, &firstDescendants) != HASH_SUCCESS) {
        FLO_HTML_PRINT_ERROR(
            "Could not allocate & copy memory for the first descendants set\n");
        return QUERY_MEMORY_ERROR;
    }

    flo_html_resetUint16HashSet(set);

    flo_html_Uint16HashSet secondDescendants;
    if (flo_html_initUint16HashSet(&secondDescendants,
                                   firstDescendants.arrayLen) != HASH_SUCCESS) {
        flo_html_destroyUint16HashSet(&firstDescendants);
        FLO_HTML_PRINT_ERROR(
            "Could not allocate memory for the second descendants set\n");
        return QUERY_MEMORY_ERROR;
    }

    bool isFirstFilled = true;
    flo_html_Uint16HashSet *toBeFilledSet = &secondDescendants;
    flo_html_Uint16HashSet *filledSet = &firstDescendants;

    while (depth > 0 && filledSet->entries > 0) {
        for (ptrdiff_t i = 0; i < dom->parentChildLen; i++) {
            flo_html_ParentChild parentChildNode = dom->parentChilds[i];
            if (dom->nodes[parentChildNode.childID].nodeType ==
                    NODE_TYPE_DOCUMENT &&
                flo_html_containsUint16HashSet(filledSet,
                                               parentChildNode.parentID)) {
                flo_html_HashStatus status = flo_html_insertUint16HashSet(
                    toBeFilledSet, parentChildNode.childID);
                if (status != HASH_SUCCESS) {
                    flo_html_destroyUint16HashSet(&firstDescendants);
                    flo_html_destroyUint16HashSet(&secondDescendants);
                    FLO_HTML_ERROR_WITH_CODE_ONLY(
                        flo_html_hashStatusToString(status),
                        "inserting into hash set failed!\n");
                    return QUERY_MEMORY_ERROR;
                }
                if (flo_html_filterNode(parentChildNode.childID, filters,
                                        filtersLen, dom)) {
                    status = flo_html_insertUint16HashSet(
                        set, parentChildNode.childID);
                    if (status != HASH_SUCCESS) {
                        flo_html_destroyUint16HashSet(&firstDescendants);
                        flo_html_destroyUint16HashSet(&secondDescendants);
                        FLO_HTML_ERROR_WITH_CODE_ONLY(
                            flo_html_hashStatusToString(status),
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

    flo_html_destroyUint16HashSet(&firstDescendants);
    flo_html_destroyUint16HashSet(&secondDescendants);
    return QUERY_SUCCESS;
}
