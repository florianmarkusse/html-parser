#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer/parse.h"
#include "type/node.h"

#define MAX_NODE_DEPTH 1U << 7U

typedef enum { OPEN_TAG, CLOSING_TAG, TAG_NAME, TEXT } TokenizerState;

void parseNodes(const char *htmlString, Document *doc) {
    TokenizerState state = TEXT;

    // To keep track of first child.
    unsigned int stack[MAX_NODE_DEPTH];
    unsigned char currentDepth = 0;
    unsigned char previousDepth = 0;
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

                previousDepth = currentDepth;
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
                previousDepth = currentDepth;
                stack[currentDepth++] = nodeID;

                printf("Current depth: %u\n", currentDepth);
                printf("previous depth: %u\n", previousDepth);
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
                    previousDepth = currentDepth;
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
