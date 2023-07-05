#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer/parse.h"
#include "type/node.h"

#define MAX_NODE_DEPTH 1U << 7U

typedef enum { OPEN_TAG, CLOSING_TAG, TAG_NAME, TEXT } TokenizerState;

typedef enum {
    NEW_FREE,
    NEW_OPEN_TAG,
    NEW_TAG_NAME,
    NEW_ATTRS,
    NEW_OPEN_PAIRED,
    NEW_TEXT_NODE,
    NUM_STATES
} State;

typedef struct {
    unsigned int stack[MAX_NODE_DEPTH];
    unsigned int nodeDepthLen;
} __attribute__((aligned(128))) NodeDepth;

static const char *stateNames[NUM_STATES] = {
    "NEW_FREE",  "NEW_OPEN_TAG",    "NEW_TAG_NAME",
    "NEW_ATTRS", "NEW_OPEN_PAIRED", "NEW_TEXT_NODE"};

void addPairedNode(const char *tagStart, unsigned int tagLength, Document *doc,
                   unsigned int *previousNodeID, NodeDepth *stack) {
    char buffer[tagLength + 1]; // Allocate a buffer with maxLength
                                // + 1 for null-termination
    snprintf(buffer, tagLength + 1, "%s", tagStart);
    NodeType nodeType = mapStringToType(tagStart, tagLength);
    unsigned int nodeID = addNode(nodeType, doc);
    printf("Opening of paired tag:\t%s\t\tID:\t%u\n", buffer, nodeID);

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

    stack->stack[stack->nodeDepthLen] = nodeID;
    stack->nodeDepthLen++;

    *previousNodeID = nodeID;
}

void addSingleNode(const char *tagStart, unsigned int tagLength, Document *doc,
                   unsigned int *previousNodeID, NodeDepth *stack) {
    char buffer[tagLength + 1]; // Allocate a buffer with maxLength
                                // + 1 for null-termination
    snprintf(buffer, tagLength + 1, "%s", tagStart);
    NodeType nodeType = mapStringToType(tagStart, tagLength);
    unsigned int nodeID = addNode(nodeType, doc);
    printf("Opening of single tag:\t%s\t\tID:\t%u\n", buffer, nodeID);

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
}

void parse(const char *xmlString, Document *doc) {
    State state = NEW_FREE;

    unsigned int currentPosition = 0;

    unsigned int tagNameStart = 0;
    unsigned int tagLength = 0;

    unsigned char isExclam = 0;

    NodeDepth stack;
    stack.nodeDepthLen = 0;

    unsigned int previousNodeID = 0;
    char ch = xmlString[currentPosition];
    while (ch != '\0') {
        printf("Current state: %s\twith char %c\n", stateNames[state], ch);

        switch (state) {
        case NEW_FREE:
            if (ch == '<') {
                state = NEW_OPEN_TAG;
            }
            break;
        case NEW_OPEN_TAG:
            if (ch == '/') {
                previousNodeID = stack.stack[stack.nodeDepthLen - 1];
                stack.nodeDepthLen--;
                state = NEW_FREE;
            }
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                tagNameStart = currentPosition;
                state = NEW_TAG_NAME;
            }
            if (ch == '!') {
                isExclam = 1;
                tagNameStart = currentPosition;
                state = NEW_TAG_NAME;
            }
            break;
        case NEW_TAG_NAME:
            if (ch == ' ') {
                tagLength = currentPosition - tagNameStart;
                state = NEW_ATTRS;
            }
            if (ch == '>') {
                tagLength = currentPosition - tagNameStart;
                addPairedNode(&xmlString[tagNameStart], tagLength, doc,
                              &previousNodeID, &stack);
                state = NEW_OPEN_PAIRED;
            }
            break;
        case NEW_ATTRS:
            if (ch == '/' || (isExclam && ch == '>')) {
                addSingleNode(&xmlString[tagNameStart], tagLength, doc,
                              &previousNodeID, &stack);
                isExclam = 0;
                state = NEW_FREE;
            } else if (ch == '>') {
                addPairedNode(&xmlString[tagNameStart], tagLength, doc,
                              &previousNodeID, &stack);
                state = NEW_OPEN_PAIRED;
            }
            break;
        case NEW_OPEN_PAIRED:
            if (ch == '<') {
                state = NEW_OPEN_TAG;
            }
            if ((ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z')) {
                state = NEW_TEXT_NODE;
            }
            break;
        case NEW_TEXT_NODE:
            if (ch == '<') {
                state = NEW_OPEN_TAG;
            }
            break;
        default:;
        }

        ch = xmlString[++currentPosition];
    }
    free((void *)xmlString);
}
