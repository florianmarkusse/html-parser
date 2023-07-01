#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type/node.h"

typedef enum { OPEN_TAG, CLOSING_TAG, TAG_NAME, TEXT } TokenizerState;

typedef struct {
    unsigned int nodeID;
    NodeType nodeType;
} __attribute__((aligned(8))) Node;

#define NODES_PER_PAGE (1 << 10)
#define PAGE_SIZE (NODES_PER_PAGE * sizeof(Node))

static Node *nodes = NULL;
static unsigned int nodeLen = 0;

Node *createNode(NodeType nodeType) {
    Node *newNode = &nodes[nodeLen];
    newNode->nodeID = nodeLen;
    newNode->nodeType = nodeType;
    nodeLen++;
    return newNode;
}

void parseNodes(const char *htmlString) {
    if (nodes == NULL) {
        nodes = malloc(PAGE_SIZE);
    }

    TokenizerState state = TEXT;

    int currentPosition = 0;
    int tokenStart = 0;
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
            } else {
                // Transition to tag name state
                state = TAG_NAME;
                tokenStart = currentPosition;
            }
            break;

        case TAG_NAME:
            if (htmlString[currentPosition] == '>' ||
                htmlString[currentPosition] == ' ') {
                int tokenLength = currentPosition - tokenStart;
                NodeType nodeType =
                    mapStringToType(&htmlString[tokenStart], tokenLength);
                Node *newNode = createNode(nodeType);

                state = TEXT;
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

void printNodes() {
    for (unsigned int i = 0; i < nodeLen; i++) {
        printf("I am node ID: %u of type: %s\n", nodes[i].nodeID,
               mapTypeToString(nodes[i].nodeType));
    }
}

void cleanup() {
    // Free the dynamically allocated memory when it is no longer needed
    free((void *)nodes);
}
