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
    const char *start;
    size_t len;
} __attribute__((aligned(16))) Property;

typedef struct {
    Property stack[MAX_ATTRIBUTES];
    size_t len;
} __attribute__((packed)) __attribute__((aligned(128))) PropertyStack;

typedef struct {
    node_id stack[MAX_NODE_DEPTH];
    node_id len;
} __attribute__((packed)) __attribute__((aligned(128))) NodeDepth;

unsigned char isAlphaBetical(const char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

DocumentStatus addToDocument(const char *tagStart, size_t tagLength,
                             Document *doc, node_id *previousNodeID,
                             NodeDepth *depthStack,
                             const unsigned char isPaired,
                             PropertyStack *binaryPropertiesStack,
                             node_id *newNodeID) {
    element_id tagID = 0;
    if (elementToIndex(&globalTags, tagStart, tagLength, isPaired, &tagID) !=
        ELEMENT_SUCCESS) {
        return DOCUMENT_NO_ELEMENT;
    }
    if (addNode(newNodeID, tagID, doc) != DOCUMENT_SUCCESS) {
        return DOCUMENT_NO_ADD;
    }

    for (size_t i = 0; i < binaryPropertiesStack->len; i++) {
        Property attr = binaryPropertiesStack->stack[i];

        element_id attributeID = 0;

        if (elementToIndex(&globalProperties, attr.start, attr.len, 0,
                           &attributeID) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
        if (addAttributeNode(*newNodeID, attributeID, doc) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
    }
    binaryPropertiesStack->len = 0;

    if (newNodeID > 0 && *previousNodeID > 0) {
        if (depthStack->len == 0) {
            if (addNextNode(*previousNodeID, *newNodeID, doc) !=
                DOCUMENT_SUCCESS) {
                return DOCUMENT_NO_ADD;
            }
        } else {
            const unsigned int parentNodeID =
                depthStack->stack[depthStack->len - 1];
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
                             PropertyStack *singlePropertyStack,
                             node_id *newNodeID) {
    if (depthStack->len >= MAX_NODE_DEPTH) {
        PRINT_ERROR("Max document node depth %u reached.\n", MAX_NODE_DEPTH);
        PRINT_ERROR("At tag %s.\n", tagStart);
        return DOCUMENT_TOO_DEEP;
    }

    DocumentStatus documentStatus =
        addToDocument(tagStart, tagLength, doc, previousNodeID, depthStack, 1,
                      singlePropertyStack, newNodeID);
    if (documentStatus != DOCUMENT_SUCCESS) {
        return documentStatus;
    }

    depthStack->stack[depthStack->len] = *newNodeID;
    depthStack->len++;

    return DOCUMENT_SUCCESS;
}

DocumentStatus putPropertyOnStack(size_t *currentStackLen,
                                  Property currentAttrs[MAX_ATTRIBUTES],
                                  const size_t start, const size_t attributeEnd,
                                  const char *xmlString) {
    if (*currentStackLen >= MAX_ATTRIBUTES) {
        PRINT_ERROR("Max number of %u attributes per tag reached.\n",
                    MAX_ATTRIBUTES);
        Property attr = currentAttrs[*currentStackLen - 1];

        char buffer[attr.len];
        strncpy(buffer, attr.start, attr.len);
        buffer[attr.len] = '\0';
        PRINT_ERROR("Property before size was %s.\n", buffer);
        return DOCUMENT_TOO_MANY_ATTRIBUTES;
    }

    Property *attr = &currentAttrs[*currentStackLen];
    attr->start = &xmlString[start];
    attr->len = attributeEnd - start + 1;
    (*currentStackLen)++;

    return DOCUMENT_SUCCESS;
}

// DocumentStatus putPropertyValueOnStack(PropertyValueStack *attributeValues,
//                                         const size_t start,
//                                         const size_t attributeEnd,
//                                         const size_t valueStart,
//                                         const size_t valueEnd) {
//     if (attributeValues->len >= MAX_ATTRIBUTES) {
//         PRINT_ERROR("Max number of %u attribute-values per tag reached.\n",
//                     MAX_ATTRIBUTES);
//         PropertyValue attr =
//             attributeValues->stack[attributeValues->len - 1];
//
//         char attributeBuffer[attr.attribute.len];
//         strncpy(attributeBuffer, attr.attribute.start,
//                 attr.attribute.len);
//         attributeBuffer[attr.attribute.len] = '\0';
//         PRINT_ERROR("Last read attribute was %s.\n", attributeBuffer);
//
//         char valueBuffer[attr.value.len];
//         strncpy(valueBuffer, attr.value.start,
//                 attr.value.len);
//         valueBuffer[attr.value.len] = '\0';
//         PRINT_ERROR("Last read value was %s.\n", valueBuffer);
//         return DOCUMENT_TOO_MANY_ATTRIBUTES;
//     }
//
//    attributeValues->stack[attributeValues->len].len =
//        currentPosition - start + 1;
//    const char *startPos =
//        attributeValues->stack[attributeValues->len].start;
//    size_t singleLen =
//        attributeValues->stack[attributeValues->len].attribute;
//    attributeValues->len++;
//
//  return DOCUMENT_SUCCESS;
// }

DocumentStatus parse(const char *xmlString, Document *doc) {
    State state = FREE;

    size_t currentPosition = 0;

    size_t tagNameStart = 0;
    size_t tagLength = 0;

    unsigned char isExclam = 0;

    NodeDepth depthStack;
    depthStack.len = 0;

    PropertyStack binaryProperties;
    binaryProperties.len = 0;

    PropertyStack attributeKeys;
    attributeKeys.len = 0;
    PropertyStack attributeValues;
    attributeValues.len = 0;

    size_t propertyStart = 0;

    DocumentStatus documentStatus = DOCUMENT_SUCCESS;

    node_id newNodeID = 0;
    node_id previousNodeID = 0;
    char ch = xmlString[currentPosition];
    char nextChar = 0;
    while (ch != '\0') {
        if (xmlString[currentPosition + 1] != '\0') {
            nextChar = xmlString[currentPosition + 1];
        } else {
            nextChar = ch;
        }

        switch (state) {
        case FREE:
            if (ch == '<') {
                state = OPEN_TAG;
            }
            break;
        case OPEN_TAG:
            if (ch == '/') {
                previousNodeID = depthStack.stack[depthStack.len - 1];
                depthStack.len--;
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
                    &depthStack, &binaryProperties, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTRS:
            if (isAlphaBetical(ch)) {
                propertyStart = currentPosition;
                if (nextChar == ' ' || nextChar == '>') {
                    documentStatus = putPropertyOnStack(
                        &binaryProperties.len, binaryProperties.stack,
                        propertyStart, currentPosition, xmlString);
                } else if (nextChar == '=') {
                    // TODO(florian): set attribute key end here.
                    documentStatus = putPropertyOnStack(
                        &attributeKeys.len, attributeKeys.stack, propertyStart,
                        currentPosition, xmlString);
                    currentPosition += 2; // skip '="'
                    state = ATTR_VALUE;
                } else {
                    state = ATTR_KEY;
                }
            }
            if (ch == '/' || (isExclam && ch == '>')) {
                documentStatus = addToDocument(
                    &xmlString[tagNameStart], tagLength, doc, &previousNodeID,
                    &depthStack, 0, &binaryProperties, &newNodeID);
                isExclam = 0;
                state = FREE;
            } else if (ch == '>') {
                documentStatus = addPairedNode(
                    &xmlString[tagNameStart], tagLength, doc, &previousNodeID,
                    &depthStack, &binaryProperties, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTR_KEY: {
            char nextChar = xmlString[currentPosition + 1];
            if (nextChar == '=') {
                // TODO(florian): set attribute key end here.
                documentStatus = putPropertyOnStack(
                    &attributeKeys.len, attributeKeys.stack, propertyStart,
                    currentPosition, xmlString);
                currentPosition += 2; // skip '="'
                state = ATTR_VALUE;
            }
            if (nextChar == ' ' || nextChar == '>') {
                documentStatus = putPropertyOnStack(
                    &binaryProperties.len, binaryProperties.stack,
                    propertyStart, currentPosition, xmlString);
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
