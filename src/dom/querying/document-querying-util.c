#include <stdlib.h>

#include "dom/querying/document-querying-util.h"
#include "dom/querying/document-querying.h"
#include "type/element/elements.h"

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
            if (*len >= *currentCap) {
                size_t newCapacity = *(currentCap) << 1U;
                if ((*results = realloc(*results, sizeof(element_id) *
                                                      newCapacity)) == NULL) {
                    return QUERYING_MEMORY_ERROR;
                }
                *currentCap = newCapacity;
            }
            (*results)[(*len)++] = doc->nodes[i].nodeID;
        }
    }

    return QUERYING_SUCCESS;
}

// TODO(florian): probably want to add a parent-child table to make this sort of
// performant.
QueryingStatus getDescendantsOf(element_id tagID, const Document *doc,
                                node_id **results, size_t *len,
                                size_t *currentCap) {
    element_id *ancestors = NULL;
    size_t ancestorLen = 0;
    size_t ancestorCap = INITIAL_QUERY_CAP;
    getNodesWithTagID(tagID, doc, &ancestors, &ancestorLen, &ancestorCap);

    if (*results == NULL) {
        *results = malloc(sizeof(node_id) * *(currentCap));
        if (*results == NULL) {
            return QUERYING_MEMORY_ERROR;
        }
    }

    for (size_t i = 0; i < doc->nodeLen; i++) {
        if (doc->nodes[i].tagID == tagID) {
            if (*len >= *currentCap) {
                size_t newCapacity = *(currentCap) << 1U;
                if ((*results = realloc(*results, sizeof(element_id) *
                                                      newCapacity)) == NULL) {
                    return QUERYING_MEMORY_ERROR;
                }
                *currentCap = newCapacity;
            }
            (*results)[(*len)++] = doc->nodes[i].nodeID;
        }
    }

    return QUERYING_SUCCESS;
}
