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

    // To keep track of previous node.
    unsigned int previousNodeID = 0;

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

                currentDepth--;
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

                if (currentDepth > 0 &&
                    stack[currentDepth - 1] == previousNodeID) {
                    addParentFirstChild(stack[currentDepth - 1], nodeID, doc);
                } else if (nodeID != 0 &&
                           (currentDepth == 0 ||
                            stack[currentDepth - 1] != previousNodeID)) {
                    addNextNode(previousNodeID, nodeID, doc);
                }

                if (!isSelfClosing(nodeType)) {
                    stack[currentDepth++] = nodeID;
                }

                state = TEXT;
                previousNodeID = nodeID;
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
