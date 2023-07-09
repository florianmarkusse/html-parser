#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "tokenizer/parse.h"
#include "utils/print/error.h"

#define MAX_NODE_DEPTH 1U << 7U
#define MAX_ATTRIBUTES 1U << 5U

typedef enum {
    FREE,
    OPEN_TAG,
    TAG_NAME,
    ATTRS,
    ATTR_KEY,
    ATTR_VALUE,
    OPEN_PAIRED,
    TEXT_NODE,
    NUM_STATES
} State;

typedef struct {
    size_t attributeStart;
    size_t attributeEnd;
} __attribute__((aligned(16))) SingleAttribute;

typedef struct {
    SingleAttribute stack[MAX_ATTRIBUTES];
    size_t stackLen;
} __attribute__((aligned(128))) SingleAttributeStack;

typedef struct {
    node_id stack[MAX_NODE_DEPTH];
    node_id stackLen;
} __attribute__((packed)) __attribute__((aligned(128))) NodeDepth;

unsigned char isAlphaBetical(const char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

DocumentStatus addToDocument(const char *tagStart, size_t tagLength,
                             Document *doc, node_id *previousNodeID,
                             NodeDepth *depthStack,
                             const unsigned char isPaired, node_id *newNodeID) {
    element_id tagID = 0;
    if (elementToIndex(&globalTags, tagStart, tagLength, isPaired, &tagID) !=
        ELEMENT_SUCCESS) {
        return DOCUMENT_NO_ELEMENT;
    }
    if (addNode(newNodeID, tagID, doc) != DOCUMENT_SUCCESS) {
        return DOCUMENT_NO_ADD;
    }

    if (newNodeID > 0 && *previousNodeID > 0) {
        if (depthStack->stackLen == 0) {
            if (addNextNode(*previousNodeID, *newNodeID, doc) !=
                DOCUMENT_SUCCESS) {
                return DOCUMENT_NO_ADD;
            }
        } else {
            const unsigned int parentNodeID =
                depthStack->stack[depthStack->stackLen - 1];
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
                             NodeDepth *depthStack, node_id *newNodeID) {
    if (depthStack->stackLen >= MAX_NODE_DEPTH) {
        PRINT_ERROR("Max document node depth %u reached.\n", MAX_NODE_DEPTH);
        PRINT_ERROR("At tag %s.\n", tagStart);
        return DOCUMENT_TOO_DEEP;
    }

    DocumentStatus documentStatus = addToDocument(
        tagStart, tagLength, doc, previousNodeID, depthStack, 1, newNodeID);
    if (documentStatus != DOCUMENT_SUCCESS) {
        return documentStatus;
    }

    depthStack->stack[depthStack->stackLen] = *newNodeID;
    depthStack->stackLen++;

    return DOCUMENT_SUCCESS;
}

void putSingleAttributeOnStack(SingleAttributeStack *singleAttributes,
                               const size_t currentPosition,
                               const char *xmlString) {
    singleAttributes->stack[singleAttributes->stackLen].attributeEnd =
        currentPosition;
    size_t startPos =
        singleAttributes->stack[singleAttributes->stackLen].attributeStart;
    size_t singleLen = currentPosition - startPos + 1;
    printf("found length %zu\n", singleLen);
    char buffer[singleLen + 1];
    strncpy(buffer, &xmlString[startPos], singleLen);
    buffer[singleLen] = '\0';
    printf("attribute: %s\n", buffer);
    singleAttributes->stackLen++;
}

DocumentStatus parse(const char *xmlString, Document *doc) {
    State state = FREE;

    size_t currentPosition = 0;

    size_t tagNameStart = 0;
    size_t tagLength = 0;

    unsigned char isExclam = 0;

    NodeDepth depthStack;
    depthStack.stackLen = 0;

    SingleAttributeStack singleAttributes;
    singleAttributes.stackLen = 0;

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
                previousNodeID = depthStack.stack[depthStack.stackLen - 1];
                depthStack.stackLen--;
                state = FREE;
            }
            if (isAlphaBetical(ch)) {
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
                                  &previousNodeID, &depthStack, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTRS:
            if (isAlphaBetical(ch)) {
                singleAttributes.stack[singleAttributes.stackLen]
                    .attributeStart = currentPosition;

                char nextChar = xmlString[currentPosition + 1];
                if (nextChar == ' ' || nextChar == '>') {
                    putSingleAttributeOnStack(&singleAttributes,
                                              currentPosition, xmlString);
                } else if (nextChar == '=') {
                    // TODO(florian): set attribute key end here.
                    currentPosition += 2; // skip '="'
                    state = ATTR_VALUE;
                } else {
                    state = ATTR_KEY;
                }
            }
            if (ch == '/' || (isExclam && ch == '>')) {
                documentStatus =
                    addToDocument(&xmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &depthStack, 0, &newNodeID);
                isExclam = 0;
                state = FREE;
            } else if (ch == '>') {
                documentStatus =
                    addPairedNode(&xmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &depthStack, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTR_KEY: {
            printf("%c\n", ch);
            char nextChar = xmlString[currentPosition + 1];
            if (nextChar == '=') {
                // TODO(florian): set attribute key end here.
                currentPosition += 2; // skip '="'
                state = ATTR_VALUE;
            }
            if (nextChar == ' ' || nextChar == '>') {
                putSingleAttributeOnStack(&singleAttributes, currentPosition,
                                          xmlString);
                state = ATTRS;
            }
            break;
        }
        case ATTR_VALUE: {
            if (ch == '"') {
                // TODO(florian): actually add this to a key-value attribute
                // stack.
                state = ATTRS;
            }
            break;
        }
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
