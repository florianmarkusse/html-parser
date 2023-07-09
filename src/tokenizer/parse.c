#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer/parse.h"
#include "utils/print/error.h"

#define MAX_NODE_DEPTH 1U << 7U

typedef enum {
    FREE,
    OPEN_TAG,
    TAG_NAME,
    ATTRS,
    OPEN_PAIRED,
    TEXT_NODE,
    NUM_STATES
} State;

typedef struct {
    node_id stack[MAX_NODE_DEPTH];
    node_id nodeDepthLen;
} __attribute__((packed)) __attribute__((aligned(128))) NodeDepth;

DocumentStatus addToDocument(const char *tagStart, size_t tagLength,
                             Document *doc, node_id *previousNodeID,
                             NodeDepth *stack, const unsigned char isPaired,
                             node_id *newNodeID) {
    element_id tagID = 0;
    if (elementToIndex(&globalTags, tagStart, tagLength, isPaired, &tagID) !=
        ELEMENT_SUCCESS) {
        return DOCUMENT_NO_TAG;
    }
    if (addNode(newNodeID, tagID, doc) != DOCUMENT_SUCCESS) {
        return DOCUMENT_NO_ADD;
    }

    if (newNodeID > 0 && *previousNodeID > 0) {
        if (stack->nodeDepthLen == 0) {
            if (addNextNode(*previousNodeID, *newNodeID, doc) !=
                DOCUMENT_SUCCESS) {
                return DOCUMENT_NO_ADD;
            }
        } else {
            const unsigned int parentNodeID =
                stack->stack[stack->nodeDepthLen - 1];
            if (parentNodeID == *previousNodeID) {
                if (addParentFirstChild(parentNodeID, *newNodeID, doc) !=
                    DOCUMENT_SUCCESS) {
                    return DOCUMENT_NO_ADD;
                }
            } else {
                if (addNextNode(*previousNodeID, *newNodeID, doc) !=
                    DOCUMENT_SUCCESS) {
                    return DOCUMENT_NO_ADD;
                }
            }
        }
    }
    *previousNodeID = *newNodeID;

    return DOCUMENT_SUCCESS;
}

DocumentStatus addPairedNode(const char *tagStart, size_t tagLength,
                             Document *doc, node_id *previousNodeID,
                             NodeDepth *stack, node_id *newNodeID) {
    if (stack->nodeDepthLen >= MAX_NODE_DEPTH) {
        PRINT_ERROR("Max document node depth %u reached.\n", MAX_NODE_DEPTH);
        PRINT_ERROR("At tag %s.\n", tagStart);
        return DOCUMENT_TOO_DEEP;
    }

    DocumentStatus documentStatus = addToDocument(
        tagStart, tagLength, doc, previousNodeID, stack, 1, newNodeID);
    if (documentStatus != DOCUMENT_SUCCESS) {
        return documentStatus;
    }

    stack->stack[stack->nodeDepthLen] = *newNodeID;
    stack->nodeDepthLen++;

    return DOCUMENT_SUCCESS;
}

DocumentStatus parse(const char *xmlString, Document *doc) {
    State state = FREE;

    size_t currentPosition = 0;

    size_t tagNameStart = 0;
    size_t tagLength = 0;

    unsigned char isExclam = 0;

    NodeDepth stack;
    stack.nodeDepthLen = 0;

    DocumentStatus documentStatus = DOCUMENT_SUCCESS;

    node_id newNodeID = 0;
    node_id previousNodeID = 0;
    char ch = xmlString[currentPosition];
    while (ch != '\0') {
        switch (state) {
        case FREE:
            if (ch == '<') {
                state = OPEN_TAG;
            }
            break;
        case OPEN_TAG:
            if (ch == '/') {
                previousNodeID = stack.stack[stack.nodeDepthLen - 1];
                stack.nodeDepthLen--;
                state = FREE;
            }
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                tagNameStart = currentPosition;
                state = TAG_NAME;
            }
            if (ch == '!') {
                isExclam = 1;
                tagNameStart = currentPosition;
                state = TAG_NAME;
            }
            break;
        case TAG_NAME:
            if (ch == ' ') {
                tagLength = currentPosition - tagNameStart;
                state = ATTRS;
            }
            if (ch == '>') {
                tagLength = currentPosition - tagNameStart;
                documentStatus =
                    addPairedNode(&xmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &stack, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTRS:
            if (ch == '/' || (isExclam && ch == '>')) {
                documentStatus =
                    addToDocument(&xmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &stack, 0, &newNodeID);
                isExclam = 0;
                state = FREE;
            } else if (ch == '>') {
                documentStatus =
                    addPairedNode(&xmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &stack, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case OPEN_PAIRED:
            if (ch == '<') {
                state = OPEN_TAG;
            }
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                state = TEXT_NODE;
            }
            break;
        case TEXT_NODE:
            if (ch == '<') {
                state = OPEN_TAG;
            }
            break;
        default:;
        }

        if (documentStatus != DOCUMENT_SUCCESS) {
            break;
        }

        ch = xmlString[++currentPosition];
    }

    return documentStatus;
}
