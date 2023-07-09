#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dom/document.h"
#include "tokenizer/parse.h"
#include "utils/print/error.h"

#define MAX_NODE_DEPTH 1U << 7U
#define MAX_ATTRIBUTES 1U << 7U

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
    const char *attributeStart;
    size_t attributeLen;
} __attribute__((aligned(16))) SingleAttribute;

typedef struct {
    SingleAttribute stack[MAX_ATTRIBUTES];
    size_t stackLen;
} __attribute__((packed)) __attribute__((aligned(128))) SingleAttributeStack;

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
                             const unsigned char isPaired,
                             SingleAttributeStack *singleAttributesStack,
                             node_id *newNodeID) {
    element_id tagID = 0;
    if (elementToIndex(&globalTags, tagStart, tagLength, isPaired, &tagID) !=
        ELEMENT_SUCCESS) {
        return DOCUMENT_NO_ELEMENT;
    }
    if (addNode(newNodeID, tagID, doc) != DOCUMENT_SUCCESS) {
        return DOCUMENT_NO_ADD;
    }

    for (size_t i = 0; i < singleAttributesStack->stackLen; i++) {
        SingleAttribute attr = singleAttributesStack->stack[i];

        element_id attributeID = 0;
        if (elementToIndex(&globalAttributes, attr.attributeStart,
                           attr.attributeLen, 0,
                           &attributeID) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
        if (addAttributeNode(*newNodeID, attributeID, doc) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
    }
    singleAttributesStack->stackLen = 0;

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
                             NodeDepth *depthStack,
                             SingleAttributeStack *singleAttributeStack,
                             node_id *newNodeID) {
    if (depthStack->stackLen >= MAX_NODE_DEPTH) {
        PRINT_ERROR("Max document node depth %u reached.\n", MAX_NODE_DEPTH);
        PRINT_ERROR("At tag %s.\n", tagStart);
        return DOCUMENT_TOO_DEEP;
    }

    DocumentStatus documentStatus =
        addToDocument(tagStart, tagLength, doc, previousNodeID, depthStack, 1,
                      singleAttributeStack, newNodeID);
    if (documentStatus != DOCUMENT_SUCCESS) {
        return documentStatus;
    }

    depthStack->stack[depthStack->stackLen] = *newNodeID;
    depthStack->stackLen++;

    return DOCUMENT_SUCCESS;
}

DocumentStatus putSingleAttributeOnStack(SingleAttributeStack *singleAttributes,
                                         const size_t currentPosition,
                                         const size_t attributeStart) {
    if (singleAttributes->stackLen >= MAX_ATTRIBUTES) {
        PRINT_ERROR("Max number of %u attributes per tag reached.\n",
                    MAX_ATTRIBUTES);
        SingleAttribute attr =
            singleAttributes->stack[singleAttributes->stackLen - 1];

        char buffer[attr.attributeLen];
        strncpy(buffer, attr.attributeStart, attr.attributeLen);
        buffer[attr.attributeLen] = '\0';
        PRINT_ERROR("Attribute before size was %s.\n", buffer);
        return DOCUMENT_TOO_MANY_ATTRIBUTES;
    }
    singleAttributes->stack[singleAttributes->stackLen].attributeLen =
        currentPosition - attributeStart + 1;
    const char *startPos =
        singleAttributes->stack[singleAttributes->stackLen].attributeStart;
    size_t singleLen =
        singleAttributes->stack[singleAttributes->stackLen].attributeLen;
    singleAttributes->stackLen++;

    return DOCUMENT_SUCCESS;
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
    size_t attributeStart = 0;

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
                documentStatus = addPairedNode(
                    &xmlString[tagNameStart], tagLength, doc, &previousNodeID,
                    &depthStack, &singleAttributes, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTRS:
            if (isAlphaBetical(ch)) {
                attributeStart = currentPosition;
                singleAttributes.stack[singleAttributes.stackLen]
                    .attributeStart = &xmlString[currentPosition];

                char nextChar = xmlString[currentPosition + 1];
                if (nextChar == ' ' || nextChar == '>') {
                    documentStatus = putSingleAttributeOnStack(
                        &singleAttributes, currentPosition, attributeStart);
                } else if (nextChar == '=') {
                    // TODO(florian): set attribute key end here.
                    currentPosition += 2; // skip '="'
                    state = ATTR_VALUE;
                } else {
                    state = ATTR_KEY;
                }
            }
            if (ch == '/' || (isExclam && ch == '>')) {
                documentStatus = addToDocument(
                    &xmlString[tagNameStart], tagLength, doc, &previousNodeID,
                    &depthStack, 0, &singleAttributes, &newNodeID);
                isExclam = 0;
                state = FREE;
            } else if (ch == '>') {
                documentStatus = addPairedNode(
                    &xmlString[tagNameStart], tagLength, doc, &previousNodeID,
                    &depthStack, &singleAttributes, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTR_KEY: {
            char nextChar = xmlString[currentPosition + 1];
            if (nextChar == '=') {
                // TODO(florian): set attribute key end here.
                currentPosition += 2; // skip '="'
                state = ATTR_VALUE;
            }
            if (nextChar == ' ' || nextChar == '>') {
                documentStatus = putSingleAttributeOnStack(
                    &singleAttributes, currentPosition, attributeStart);
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
