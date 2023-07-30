#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dom/querying/document-querying-util.h"
#include "dom/querying/document-querying.h"
#include "type/element/elements.h"
#include "type/node/parent-child.h"
#include "utils/memory/memory.h"
#include "utils/print/error.h"

QueryingStatus getTagID(const char *tag, element_id *tagID) {
    if (findElement(&gTags.container, &gTags.pairedLen, tag, 0, tagID) ==
        ELEMENT_SUCCESS) {
        return QUERYING_SUCCESS;
    }

    if (findElement(&gTags.container, &gTags.singleLen, tag, SINGLES_OFFSET,
                    tagID) == ELEMENT_SUCCESS) {
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
        if (doc->nodes[results[i] - 1].tagID == tagID) {
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
                                size_t *currentCap, Document *doc) {
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

    size_t nodesAdded = *len;
    *len = 0;
    size_t startLen = 0;

    while (nodesAdded > 0) {
        if (nodesAdded > parentCap) {
            if ((parents = resizeArray(parents, nodesAdded, &parentCap,
                                       sizeof(node_id), parentCap)) == NULL) {
                free(parents);
                return QUERYING_MEMORY_ERROR;
            }
        }
        memcpy(parents, (*results) + startLen, nodesAdded * sizeof(node_id));
        parentLen = nodesAdded;

        startLen = *len;
        for (size_t i = 0; i < doc->parentChildLen; i++) {
            ParentChild parentChildNode = doc->parentChilds[i];
            if (!(isText(doc->nodes[parentChildNode.childID - 1].tagID)) &&
                isParentIn(parentChildNode.parentID, parents, parentLen)) {
                if ((*(results) = resizeArray(*results, *len, currentCap,
                                              sizeof(node_id), *currentCap)) ==
                    NULL) {
                    free(parents);
                    return QUERYING_MEMORY_ERROR;
                }

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

        nodesAdded = *len - startLen;
    }

    free(parents);
    return QUERYING_SUCCESS;
}
