#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dom/querying/document-querying-util.h"
#include "utils/memory/memory.h"
#include "utils/print/error.h"

bool filterNode(const node_id nodeID, FilterType *filters,
                const size_t filterslen, const Document *doc) {
    for (size_t i = 0; i < filterslen; i++) {
        FilterType filterType = filters[i];
        switch (filterType.attributeSelector) {
        case TAG: {
            if (doc->nodes[nodeID].tagID != filterType.data.tagID) {
                return false;
            }
            break;
        }
        case BOOLEAN_PROPERTY: {
            bool hasBoolProp = false;
            // TODO(florian): find way to improve this.
            for (size_t j = 0; j < doc->boolPropsLen; j++) {
                if (doc->boolProps[j].nodeID == nodeID &&
                    doc->boolProps[j].propID == filterType.data.propID) {
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
            for (size_t j = 0; j < doc->propsLen; j++) {
                if (doc->props[j].nodeID == nodeID &&
                    doc->props[j].keyID == filterType.data.keyValuePair.keyID &&
                    doc->props[j].valueID ==
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

QueryingStatus getTagID(const char *tag, element_id *tagID,
                        const DataContainer *dataContainer) {
    if (findElement(&dataContainer->tags.container,
                    &dataContainer->tags.pairedLen, tag, 0,
                    tagID) == ELEMENT_SUCCESS) {
        return QUERYING_SUCCESS;
    }

    if (findElement(&dataContainer->tags.container,
                    &dataContainer->tags.singleLen, tag, SINGLES_OFFSET,
                    tagID) == ELEMENT_SUCCESS) {
        return QUERYING_SUCCESS;
    }

    return QUERYING_NOT_FOUND;
}

QueryingStatus getBoolPropID(const char *tag, element_id *propID,
                             const DataContainer *dataContainer) {
    if (findElement(&dataContainer->propKeys.container,
                    &dataContainer->propKeys.singleLen, tag, SINGLES_OFFSET,
                    propID) == ELEMENT_SUCCESS) {
        return QUERYING_SUCCESS;
    }

    return QUERYING_NOT_FOUND;
}

QueryingStatus getKeyPropID(const char *tag, element_id *keyID,
                            const DataContainer *dataContainer) {
    if (findElement(&dataContainer->propKeys.container,
                    &dataContainer->propKeys.pairedLen, tag, 0,
                    keyID) == ELEMENT_SUCCESS) {
        return QUERYING_SUCCESS;
    }

    return QUERYING_NOT_FOUND;
}

QueryingStatus getValuePropID(const char *tag, element_id *valueID,
                              const DataContainer *dataContainer) {
    if (findElement(&dataContainer->propValues.container,
                    &dataContainer->propValues.len, tag, 0,
                    valueID) == ELEMENT_SUCCESS) {
        return QUERYING_SUCCESS;
    }

    return QUERYING_NOT_FOUND;
}

QueryingStatus getNodesWithTagID(element_id tagID, const Document *doc,
                                 node_id **results, size_t *len,
                                 size_t *currentCap) {
    if (*results == NULL) {
        *results = malloc(sizeof(node_id) * *(currentCap));
        if (*results == NULL) {
            return QUERYING_MEMORY_ERROR;
        }
    }

    for (size_t i = 0; i < doc->nodeLen; i++) {
        if (doc->nodes[i].tagID == tagID) {
            if ((*(results) = resizeArray(*results, *len, currentCap,
                                          sizeof(node_id), *currentCap)) ==
                NULL) {
                return QUERYING_MEMORY_ERROR;
            }
            (*results)[(*len)++] = doc->nodes[i].nodeID;
        }
    }

    return QUERYING_SUCCESS;
}

QueryingStatus filterByTagID(const element_id tagID, const Document *doc,
                             node_id *results, size_t *len) {
    size_t nextFreeSpot = 0;
    for (size_t i = 0; i < *len; i++) {
        if (doc->nodes[results[i]].tagID == tagID) {
            results[nextFreeSpot++] = results[i];
        }
    }

    *len = nextFreeSpot;
    return QUERYING_SUCCESS;
}

unsigned char isParentIn(const node_id parentID, const node_id *array,
                         const size_t arrayLen) {
    for (size_t j = 0; j < arrayLen; j++) {
        if (parentID == array[j]) {
            return 1;
        }
    }

    return 0;
}

QueryingStatus getDescendantsOf(node_id **results, size_t *len,
                                size_t *currentCap, const Document *doc,
                                size_t depth) {
    if (*results == NULL) {
        PRINT_ERROR("Provide the node ID(s) of which you want all the "
                    "descendants of in the results array.\n");
        return QUERYING_MEMORY_ERROR;
    }

    node_id *parents = malloc(*currentCap * sizeof(node_id));
    size_t parentLen = *len;
    size_t parentCap = *currentCap;
    if (parents == NULL) {
        PRINT_ERROR("Could not allocate memory for the parents array.\n");
        return QUERYING_MEMORY_ERROR;
    }

    size_t nodesFound = *len;
    *len = 0;
    size_t startLen = 0;

    while (depth > 0 && nodesFound > 0) {
        if (nodesFound > parentCap) {
            if ((parents = resizeArray(parents, nodesFound, &parentCap,
                                       sizeof(node_id), parentCap)) == NULL) {
                free(parents);
                return QUERYING_MEMORY_ERROR;
            }
        }
        memcpy(parents, (*results) + startLen, nodesFound * sizeof(node_id));
        parentLen = nodesFound;

        startLen = *len;
        for (size_t i = 0; i < doc->parentChildLen; i++) {
            ParentChild parentChildNode = doc->parentChilds[i];
            if (!(isText(doc->nodes[parentChildNode.childID].tagID)) &&
                isParentIn(parentChildNode.parentID, parents, parentLen)) {
                if ((*(results) = resizeArray(*results, *len, currentCap,
                                              sizeof(node_id), *currentCap)) ==
                    NULL) {
                    free(parents);
                    return QUERYING_MEMORY_ERROR;
                }

                nodesFound++;

                // TODO(florian): not very nice way of doing this. Use a hash or
                // something.
                unsigned char isDuplicate = 0;
                for (size_t i = 0; i < *len; i++) {
                    if ((*results)[i] == parentChildNode.childID) {
                        isDuplicate = 1;
                        break;
                    }
                }

                if (!isDuplicate) {
                    (*results)[(*len)++] = parentChildNode.childID;
                }
            }
        }

        depth--;
    }

    free(parents);
    return QUERYING_SUCCESS;
}
