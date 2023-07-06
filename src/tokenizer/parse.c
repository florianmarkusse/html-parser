#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer/parse.h"
#include "type/node-tag.h"

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
} __attribute__((aligned(128))) NodeDepth;

node_id addToDocument(const char *tagStart, unsigned int tagLength,
                      Document *doc, unsigned int *previousNodeID,
                      NodeDepth *stack, const unsigned char isPaired) {
    char buffer[PAGE_SIZE]; // Allocate a buffer with maxLength
                            // + 1 for null-termination
    snprintf(buffer, tagLength + 1, "%s", tagStart);
    tag_id tagID = tagToIndex(buffer, isPaired);
    node_id nodeID = addNode(tagID, doc);

    if (nodeID > 0 && *previousNodeID > 0) {
        if (stack->nodeDepthLen == 0) {
            addNextNode(*previousNodeID, nodeID, doc);
        } else {
            const unsigned int parentNodeID =
                stack->stack[stack->nodeDepthLen - 1];
            if (parentNodeID == *previousNodeID) {
                addParentFirstChild(parentNodeID, nodeID, doc);
            } else {
                addNextNode(*previousNodeID, nodeID, doc);
            }
        }
    }
    *previousNodeID = nodeID;

    printf("tag:\t%s\twith ID:\t%u\twith tag type ID:\t%hu\n", buffer, nodeID,
           tagID);

    return nodeID;
}

void addPairedNode(const char *tagStart, unsigned int tagLength, Document *doc,
                   unsigned int *previousNodeID, NodeDepth *stack) {
    stack->stack[stack->nodeDepthLen] =
        addToDocument(tagStart, tagLength, doc, previousNodeID, stack, 1);
    stack->nodeDepthLen++;
}

void parse(const char *xmlString, Document *doc) {
    State state = FREE;

    unsigned int currentPosition = 0;

    unsigned int tagNameStart = 0;
    unsigned int tagLength = 0;

    unsigned char isExclam = 0;

    NodeDepth stack;
    stack.nodeDepthLen = 0;

    unsigned int previousNodeID = 0;
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
                addPairedNode(&xmlString[tagNameStart], tagLength, doc,
                              &previousNodeID, &stack);
                state = OPEN_PAIRED;
            }
            break;
        case ATTRS:
            if (ch == '/' || (isExclam && ch == '>')) {
                addToDocument(&xmlString[tagNameStart], tagLength, doc,
                              &previousNodeID, &stack, 0);
                isExclam = 0;
                state = FREE;
            } else if (ch == '>') {
                addPairedNode(&xmlString[tagNameStart], tagLength, doc,
                              &previousNodeID, &stack);
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

        ch = xmlString[++currentPosition];
    }
    free((void *)xmlString);
}
