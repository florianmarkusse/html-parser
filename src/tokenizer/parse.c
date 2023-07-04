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
    NEW_CLOSE_PAIRED,
    NEW_TAG_NAME,
    NEW_ATTRS,
    NEW_OPEN_PAIRED,
    NEW_CLOSE_SINGLE,
    NEW_TEXT_NODE,
    NUM_STATES
} State;

static const char *stateNames[NUM_STATES] = {
    "NEW_FREE",  "NEW_OPEN_TAG",    "NEW_CLOSE_PAIRED", "NEW_TAG_NAME",
    "NEW_ATTRS", "NEW_OPEN_PAIRED", "NEW_CLOSE_SINGLE", "NEW_TEXT_NODE"};

void parseNodesNew(const char *htmlString, Document *doc) {
    State state = NEW_FREE;

    unsigned int currentPosition = 0;
    unsigned int tagNameStart = 0;
    unsigned int tagLength = 0;
    unsigned char isPaired = 0;
    unsigned char isSingle = 0;
    unsigned char isExclam = 0;
    char ch = htmlString[currentPosition];
    while (ch != '\0') {
        // printf("Current state: %s\twith char %c\n", stateNames[state], ch);

        switch (state) {
        case NEW_FREE:
            if (ch == '<') {
                state = NEW_OPEN_TAG;
            }
            break;
        case NEW_OPEN_TAG:
            if (ch == '/') {
                state = NEW_CLOSE_PAIRED;
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
                state = NEW_ATTRS;
                tagLength = currentPosition - tagNameStart;
            }
            if (ch == '>') {
                state = NEW_OPEN_PAIRED;
                tagLength = currentPosition - tagNameStart;
                isPaired = 1;
            }
            break;
        case NEW_ATTRS:
            if (ch == '/') {
                state = NEW_CLOSE_SINGLE;
                isSingle = 1;
            }
            if (ch == '>') {
                state = NEW_OPEN_PAIRED;
                isPaired = 1;
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
        case NEW_CLOSE_SINGLE:
            // Is always true afaik.
            if (ch == '>') {
                state = NEW_FREE;
                // Closing of a single tag.
            }
            break;
        case NEW_CLOSE_PAIRED:
            if (ch == '>') {
                state = NEW_FREE;
            }
            break;
        default:;
        }

        if (tagLength > 0 && (isSingle | isPaired)) {
            char buffer[tagLength + 1]; // Allocate a buffer with maxLength
                                        // + 1 for null-termination
            snprintf(buffer, tagLength + 1, "%s", &htmlString[tagNameStart]);
            if (isSingle || (isExclam && isPaired)) {
                printf("Opening of singe tag:\t%s\n", buffer);
            } else {
                printf("Opening of paired tag:\t%s\n", buffer);
            }
            tagLength = 0;
            isSingle = 0;
            isPaired = 0;
            isExclam = 0;
        }
        ch = htmlString[++currentPosition];
    }
}

void parseNodes(const char *htmlString, Document *doc) {
    TokenizerState state = TEXT;

    // To keep track of first child.
    unsigned int stack[MAX_NODE_DEPTH];
    unsigned char currentDepth = 0;
    unsigned char popped = 0;

    unsigned int previousNodeID = 0;
    unsigned int previousPoppedNodeID = 0;

    // To keep track of type of node.
    unsigned int currentPosition = 0;
    unsigned int tokenStart = 0;
    while (htmlString[currentPosition] != '\0') {
        switch (state) {
        case TEXT:
            if (htmlString[currentPosition] == '<') {
                // Transition to tag open state
                state = OPEN_TAG;
                tokenStart = currentPosition;
            }
            break;

        case OPEN_TAG:
            if (htmlString[currentPosition] == '/') {
                // Transition to text state
                state = TEXT;

                previousPoppedNodeID = stack[currentDepth - 1];
                currentDepth--;
                popped = 1;
            } else {
                // Transition to tag name state
                state = TAG_NAME;
                tokenStart = currentPosition;
            }
            break;

        case TAG_NAME:
            if (htmlString[currentPosition] == '>' ||
                htmlString[currentPosition] == ' ') {
                unsigned int tokenLength = currentPosition - tokenStart;
                NodeType nodeType =
                    mapStringToType(&htmlString[tokenStart], tokenLength);

                unsigned int nodeID = addNode(nodeType, doc);
                stack[currentDepth++] = nodeID;

                printf("Current depth: %u\n", currentDepth);
                printf("Current node ID: %u\n", nodeID);
                for (int i = 0; i < currentDepth; i++) {
                    printf("stack[%i] ID: %u\n", i, stack[i]);
                }

                // [2] <- currentDepth
                // [1] <- new node ID
                // [0] <- parent of new node ID
                // stack
                if (currentDepth > 1 &&
                    stack[currentDepth - 2] == previousNodeID) {
                    addParentFirstChild(stack[currentDepth - 2], nodeID, doc);
                } else if (nodeID != 0 &&
                           (currentDepth == 1 ||
                            stack[currentDepth - 2] != previousNodeID)) {
                    if (popped) {
                        addNextNode(previousPoppedNodeID, nodeID, doc);
                    } else {
                        addNextNode(previousNodeID, nodeID, doc);
                    }
                }

                printf("\n\n");
                /*
                if (currentDepth > previousDepth) {
                    // [] <- currentDepth
                    // [2] <- new node ID
                    // [1] <- parent of new node ID
                    // [0]
                    // stack
                    addParentFirstChild(stack[currentDepth - 2], nodeID, doc);
                } else if (currentDepth == previousDepth) {
                    addNextNode(previousNodeID, nodeID, doc);
                } else if (currentDepth < previousDepth) {
                    addNextNode(previousPoppedNodeID, nodeID, doc);
                }
                */

                /*
                if (currentDepth > 1 &&
                    stack[currentDepth - 2] == previousNodeID) {
                    addParentFirstChild(stack[currentDepth - 2], nodeID, doc);
                } else if (nodeID != 0 &&
                           (currentDepth == 0 ||
                            stack[currentDepth - 1] != previousNodeID)) {
                    addNextNode(previousNodeID, nodeID, doc);
                }
                */

                if (isSelfClosing(nodeType)) {
                    currentDepth--;
                }
                state = TEXT;
                previousNodeID = nodeID;
                popped = 0;
            }
            break;

        default:
            // Other states and actions can be added here as needed
            break;
        }
        currentPosition++;
    }

    free((void *)htmlString);
}
