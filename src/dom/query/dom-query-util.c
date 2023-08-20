#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/dom/query/dom-query-util.h"
#include "flo/html-parser/type/node/next-node.h"
#include "flo/html-parser/utils/memory/memory.h"
#include "flo/html-parser/utils/print/error.h"

bool filterNode(const node_id nodeID, const FilterType *filters,
                const size_t filterslen, const Dom *dom) {
    for (size_t i = 0; i < filterslen; i++) {
        FilterType filterType = filters[i];
        switch (filterType.attributeSelector) {
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

QueryStatus getTagID(const char *tag, element_id *tagID,
                     const DataContainer *dataContainer) {
    if (findElement(&dataContainer->tags.container,
                    &dataContainer->tags.pairedLen, tag, 0,
                    tagID) == ELEMENT_SUCCESS) {
        return QUERY_SUCCESS;
    }

    if (findElement(&dataContainer->tags.container,
                    &dataContainer->tags.singleLen, tag, SINGLES_OFFSET,
                    tagID) == ELEMENT_SUCCESS) {
        return QUERY_SUCCESS;
    }

    return QUERY_NOT_SEEN_BEFORE;
}

QueryStatus getBoolPropID(const char *tag, element_id *propID,
                          const DataContainer *dataContainer) {
    if (findElement(&dataContainer->propKeys.container,
                    &dataContainer->propKeys.singleLen, tag, SINGLES_OFFSET,
                    propID) == ELEMENT_SUCCESS) {
        return QUERY_SUCCESS;
    }

    return QUERY_NOT_SEEN_BEFORE;
}

QueryStatus getKeyPropID(const char *tag, element_id *keyID,
                         const DataContainer *dataContainer) {
    if (findElement(&dataContainer->propKeys.container,
                    &dataContainer->propKeys.pairedLen, tag, 0,
                    keyID) == ELEMENT_SUCCESS) {
        return QUERY_SUCCESS;
    }

    return QUERY_NOT_SEEN_BEFORE;
}

QueryStatus getValuePropID(const char *tag, element_id *valueID,
                           const DataContainer *dataContainer) {
    if (findElement(&dataContainer->propValues.container,
                    &dataContainer->propValues.len, tag, 0,
                    valueID) == ELEMENT_SUCCESS) {
        return QUERY_SUCCESS;
    }

    return QUERY_NOT_SEEN_BEFORE;
}

QueryStatus
getNodesWithoutCombinator(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                          const size_t filtersLen, const Dom *dom,
                          node_id **results, size_t *len, size_t *currentCap) {
    for (size_t i = 0; i < dom->nodeLen; i++) {
        if (filterNode(dom->nodes[i].nodeID, filters, filtersLen, dom)) {
            if ((*(results) = resizeArray(*results, *len, currentCap,
                                          sizeof(node_id), *currentCap)) ==
                NULL) {
                return QUERY_MEMORY_ERROR;
            }
            (*results)[(*len)++] = dom->nodes[i].nodeID;
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
                     const size_t numberOfSiblings, node_id **results,
                     size_t *len, size_t *currentCap) {
    size_t filteredAdjacentsCap = *len;
    size_t filteredAdjacentsLen = 0;
    node_id *filteredAdjacents = malloc(filteredAdjacentsCap * sizeof(node_id));
    if (filteredAdjacents == NULL) {
        PRINT_ERROR(
            "Could not allocate memory for the filtered adjacents array\n");
        return QUERY_MEMORY_ERROR;
    }

    for (size_t i = 0; i < *len; i++) {
        node_id nextNodeID = getNextNode((*results)[i], dom);
        size_t siblingsNumberCopy = numberOfSiblings;
        while (siblingsNumberCopy && nextNodeID) {
            if (filterNode(nextNodeID, filters, filtersLen, dom)) {
                if (!isPresentIn(nextNodeID, filteredAdjacents,
                                 filteredAdjacentsLen)) {
                    if ((filteredAdjacents = resizeArray(
                             filteredAdjacents, filteredAdjacentsLen,
                             &filteredAdjacentsCap, sizeof(node_id),
                             filteredAdjacentsCap)) == NULL) {
                        free(filteredAdjacents);
                        PRINT_ERROR(
                            "Failed to allocate memory finding adjacent "
                            "elements!\n");
                        return QUERY_MEMORY_ERROR;
                    }

                    filteredAdjacents[filteredAdjacentsLen++] = nextNodeID;
                }
            }

            siblingsNumberCopy--;
            nextNodeID = getNextNode(nextNodeID, dom);
        }
    }

    free(*results);
    *results = filteredAdjacents;
    *len = filteredAdjacentsLen;
    *currentCap = filteredAdjacentsCap;

    return QUERY_SUCCESS;
}

QueryStatus
getFilteredDescendants(const FilterType filters[MAX_FILTERS_PER_ELEMENT],
                       const size_t filtersLen, const Dom *dom, size_t depth,
                       node_id **results, size_t *len, size_t *currentCap) {
    node_id *parents = malloc(*currentCap * sizeof(node_id));
    size_t parentLen = *len;
    size_t parentCap = *currentCap;
    if (parents == NULL) {
        PRINT_ERROR("Could not allocate memory for the parents array.\n");
        return QUERY_MEMORY_ERROR;
    }

    node_id *foundNodes = malloc(*currentCap * sizeof(node_id));
    size_t foundNodesLen = *len;
    size_t foundNodesCap = *currentCap;
    if (foundNodes == NULL) {
        free(parents);
        PRINT_ERROR("Could not allocate memory for the parents array.\n");
        return QUERY_MEMORY_ERROR;
    }
    memcpy(foundNodes, *results, *len * sizeof(node_id));

    *len = 0;
    while (depth > 0 && foundNodesLen > 0) {
        if (foundNodesLen > parentCap) {
            if ((parents = resizeArray(parents, foundNodesLen, &parentCap,
                                       sizeof(node_id), parentCap)) == NULL) {
                free(parents);
                free(foundNodes);
                return QUERY_MEMORY_ERROR;
            }
        }
        memcpy(parents, foundNodes, foundNodesLen * sizeof(node_id));
        parentLen = foundNodesLen;
        foundNodesLen = 0;
        // TODO(florian): can replace this with looping over the parents
        // array instead once a performant lookup is available.
        for (size_t i = 0; i < dom->parentChildLen; i++) {
            ParentChild parentChildNode = dom->parentChilds[i];
            if (!(isText(dom->nodes[parentChildNode.childID].tagID)) &&
                isPresentIn(parentChildNode.parentID, parents, parentLen)) {
                if ((foundNodes =
                         resizeArray(foundNodes, foundNodesLen, &foundNodesCap,
                                     sizeof(node_id), foundNodesCap)) == NULL) {
                    free(parents);
                    free(foundNodes);
                    return QUERY_MEMORY_ERROR;
                }
                foundNodes[foundNodesLen++] = parentChildNode.childID;

                if (filterNode(parentChildNode.childID, filters, filtersLen,
                               dom)) {
                    if (!isPresentIn(parentChildNode.childID, *results, *len)) {
                        if ((*(results) = resizeArray(
                                 *results, *len, currentCap, sizeof(node_id),
                                 *currentCap)) == NULL) {
                            free(parents);
                            free(foundNodes);
                            return QUERY_MEMORY_ERROR;
                        }
                        (*results)[(*len)++] = parentChildNode.childID;
                    }
                }
            }
        }

        depth--;
    }

    free(parents);
    free(foundNodes);
    return QUERY_SUCCESS;
}
