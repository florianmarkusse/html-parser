#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/utils/print/error.h"

bool filterNode(const node_id nodeID, const FilterType *filters,
                const size_t filterslen, const Dom *dom) {
    for (size_t i = 0; i < filterslen; i++) {
        FilterType filterType = filters[i];
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
            bool hasBoolProp = false;
            // TODO(florian): find way to improve this.
            for (size_t j = 0; j < dom->boolPropsLen; j++) {
                if (dom->boolProps[j].nodeID == nodeID &&
                    dom->boolProps[j].propID == filterType.data.propID) {
                    hasBoolProp = true;
                    break;
                }
            }
            if (!hasBoolProp) {
                return false;
            }
            break;
        }
        case PROPERTY: {
            bool hasProp = false;
            // TODO(florian): find way to improve this.
            for (size_t j = 0; j < dom->propsLen; j++) {
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
        default: {
            PRINT_ERROR("Unknown attribute selector in filter function\n");
            return false;
        }
        }
    }

    return true;
}

indexID getTagID(const char *tag, const DataContainer *dataContainer) {
    HashElement ignore;
    indexID tagID = 0;
    containsStringWithDataHashSet(&dataContainer->tags.set, tag, &ignore,
                                  &tagID);
    return tagID;
}

indexID getBoolPropID(const char *boolProp,
                      const DataContainer *dataContainer) {
    HashElement ignore;
    indexID propID = 0;
    containsStringWithDataHashSet(&dataContainer->boolProps.set, boolProp,
                                  &ignore, &propID);
    return propID;
}

indexID getPropKeyID(const char *keyProp, const DataContainer *dataContainer) {
    HashElement ignore;
    indexID keyID = 0;
    containsStringWithDataHashSet(&dataContainer->propKeys.set, keyProp,
                                  &ignore, &keyID);
    return keyID;
}

indexID getPropValueID(const char *valueProp,
                       const DataContainer *dataContainer) {
    HashElement ignore;
    indexID valueID = 0;
    containsStringWithDataHashSet(&dataContainer->propValues.set, valueProp,
                                  &ignore, &valueID);
    return valueID;
}

QueryStatus
getNodesWithoutCombinator(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                          const size_t filtersLen, const Dom *dom,
                          Uint16HashSet *set) {
    for (size_t i = 0; i < dom->nodeLen; i++) {
        if (filterNode(dom->nodes[i].nodeID, filters, filtersLen, dom)) {
            HashStatus status = insertUint16HashSet(set, dom->nodes[i].nodeID);
            if (status != HASH_SUCCESS) {
                ERROR_WITH_CODE_ONLY(hashStatusToString(status),
                                     "inserting into hash set failed!\n");
                return QUERY_MEMORY_ERROR;
            }
        }
    }

    return QUERY_SUCCESS;
}

QueryStatus filterByTagID(const element_id tagID, const Dom *dom,
                          node_id *results, size_t *len) {
    size_t nextFreeSpot = 0;
    for (size_t i = 0; i < *len; i++) {
        if (dom->nodes[results[i]].tagID == tagID) {
            results[nextFreeSpot++] = results[i];
        }
    }

    *len = nextFreeSpot;
    return QUERY_SUCCESS;
}

// TODO(florian): not very nice way of doing this. Use a
// hash or something.
unsigned char isPresentIn(const node_id nodeID, const node_id *array,
                          const size_t arrayLen) {
    for (size_t j = 0; j < arrayLen; j++) {
        if (nodeID == array[j]) {
            return 1;
        }
    }

    return 0;
}

QueryStatus
getFilteredAdjacents(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                     const size_t filtersLen, const Dom *dom,
                     const size_t numberOfSiblings, Uint16HashSet *set) {
    Uint16HashSet filteredAdjacents;
    if (initUint16HashSet(&filteredAdjacents, set->arrayLen) != HASH_SUCCESS) {
        PRINT_ERROR(
            "Could not allocate memory for the filtered adjacents set\n");
        return QUERY_MEMORY_ERROR;
    }

    Uint16HashSetIterator iterator;
    initUint16HashSetIterator(&iterator, set);

    while (hasNextUint16HashSetIterator(&iterator)) {
        node_id inSet = nextUint16HashSetIterator(&iterator);
        node_id nextNodeID = getNext(inSet, dom);
        size_t siblingsNumberCopy = numberOfSiblings;

        while (siblingsNumberCopy > 0 && nextNodeID > 0) {
            if (filterNode(nextNodeID, filters, filtersLen, dom)) {
                HashStatus status =
                    insertUint16HashSet(&filteredAdjacents, nextNodeID);
                if (status != HASH_SUCCESS) {
                    destroyUint16HashSet(&filteredAdjacents);
                    ERROR_WITH_CODE_ONLY(hashStatusToString(status),
                                         "inserting into hash set failed!\n");
                    return QUERY_MEMORY_ERROR;
                }
            }

            siblingsNumberCopy--;
            nextNodeID = getNext(nextNodeID, dom);
        }
    }

    destroyUint16HashSet(set);
    set->arrayLen = filteredAdjacents.arrayLen;
    set->array = filteredAdjacents.array;
    set->entries = filteredAdjacents.entries;

    return QUERY_SUCCESS;
}

QueryStatus
getFilteredDescendants(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                       const size_t filtersLen, const Dom *dom, size_t depth,
                       Uint16HashSet *set) {
    Uint16HashSet firstDescendants;
    if (copyUint16HashSet(set, &firstDescendants) != HASH_SUCCESS) {
        PRINT_ERROR(
            "Could not allocate & copy memory for the first descendants set\n");
        return QUERY_MEMORY_ERROR;
    }

    resetUint16HashSet(set);

    Uint16HashSet secondDescendants;
    if (initUint16HashSet(&secondDescendants, firstDescendants.arrayLen) !=
        HASH_SUCCESS) {
        destroyUint16HashSet(&firstDescendants);
        PRINT_ERROR(
            "Could not allocate memory for the second descendants set\n");
        return QUERY_MEMORY_ERROR;
    }

    bool isFirstFilled = true;
    Uint16HashSet *toBeFilledSet = &secondDescendants;
    Uint16HashSet *filledSet = &firstDescendants;

    while (depth > 0 && filledSet->entries > 0) {
        for (size_t i = 0; i < dom->parentChildLen; i++) {
            ParentChild parentChildNode = dom->parentChilds[i];
            if (dom->nodes[parentChildNode.childID].nodeType ==
                    NODE_TYPE_DOCUMENT &&
                containsUint16HashSet(filledSet, parentChildNode.parentID)) {
                HashStatus status =
                    insertUint16HashSet(toBeFilledSet, parentChildNode.childID);
                if (status != HASH_SUCCESS) {
                    destroyUint16HashSet(&firstDescendants);
                    destroyUint16HashSet(&secondDescendants);
                    ERROR_WITH_CODE_ONLY(hashStatusToString(status),
                                         "inserting into hash set failed!\n");
                    return QUERY_MEMORY_ERROR;
                }
                if (filterNode(parentChildNode.childID, filters, filtersLen,
                               dom)) {
                    status = insertUint16HashSet(set, parentChildNode.childID);
                    if (status != HASH_SUCCESS) {
                        destroyUint16HashSet(&firstDescendants);
                        destroyUint16HashSet(&secondDescendants);
                        ERROR_WITH_CODE_ONLY(
                            hashStatusToString(status),
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

        resetUint16HashSet(toBeFilledSet);

        depth--;
    }

    destroyUint16HashSet(&firstDescendants);
    destroyUint16HashSet(&secondDescendants);
    return QUERY_SUCCESS;
}