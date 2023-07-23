#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dom/document.h"
#include "tokenizer/parse-property.h"
#include "tokenizer/parse.h"
#include "utils/print/error.h"

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

const char *stateToString(State state) {
    static const char *stateStrings[NUM_STATES] = {
        "FREE",     "OPEN_TAG",   "TAG_NAME",    "ATTRS",
        "ATTR_KEY", "ATTR_VALUE", "OPEN_PAIRED", "TEXT_NODE"};

    if (state >= 0 && state < NUM_STATES) {
        return stateStrings[state];
    }

    return "UNKNOWN";
}

typedef struct {
    ParseProperty stack[MAX_PROPERTIES];
    size_t len;
} __attribute__((aligned(128))) ParsePropertyStack;

typedef struct {
    node_id stack[MAX_NODE_DEPTH];
    node_id len;
} __attribute__((aligned(128))) NodeDepth;

unsigned char isAlphaBetical(const char ch) {
    return (ch >= 'a' && ch <= 'z') || (ch >= 'A' && ch <= 'Z');
}

DocumentStatus
addToDocument(const char *tagStart, size_t tagLength, Document *doc,
              node_id *previousNodeID, NodeDepth *depthStack,
              const unsigned char isPaired, ParsePropertyStack *binaryProps,
              ParsePropertyStack *propKeys, ParsePropertyStack *propValues,
              node_id *newNodeID) {
    element_id tagID = 0;
    if (combinedElementToIndex(&gTags, tagStart, tagLength, isPaired, &tagID) !=
        ELEMENT_SUCCESS) {
        return DOCUMENT_NO_ELEMENT;
    }
    if (addNode(newNodeID, tagID, doc) != DOCUMENT_SUCCESS) {
        return DOCUMENT_NO_ADD;
    }

    for (size_t i = 0; i < binaryProps->len; i++) {
        ParseProperty parseProp = binaryProps->stack[i];

        element_id propID = 0;

        if (combinedElementToIndex(&gPropKeys, parseProp.start, parseProp.len,
                                   0, &propID) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
        if (addBooleanProperty(*newNodeID, propID, doc) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
    }
    binaryProps->len = 0;

    for (size_t i = 0; i < propKeys->len; i++) {
        ParseProperty key = propKeys->stack[i];
        element_id keyID = 0;
        if (combinedElementToIndex(&gPropKeys, key.start, key.len, 1, &keyID) !=
            ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }

        ParseProperty value = propValues->stack[i];
        element_id valueID = 0;
        if (elementToIndex(&gPropValues, value.start, value.len, &valueID) !=
            ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }

        if (addProperty(*newNodeID, keyID, valueID, doc) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
    }
    propKeys->len = 0;
    propValues->len = 0;

    if (!isSingle(tagID)) {
        element_id textID = 0;
        createElement(&gText.container, "test-test", &gText.len, 0, &textID);
        if (addTextNode(*newNodeID, textID, doc) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
    }

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

    char buffer[tagLength];
    strncpy(buffer, tagStart, tagLength);
    buffer[tagLength] = '\0';
    printf("Current tag: %s\n", buffer);
    printf("Given tag ID: %u\n", tagID);
    printf("Given node ID: %u\n", *newNodeID);
    printf("--------------------------------\n\n");
    return DOCUMENT_SUCCESS;
}

DocumentStatus addPairedNode(const char *tagStart, size_t tagLength,
                             Document *doc, node_id *previousNodeID,
                             NodeDepth *depthStack,
                             ParsePropertyStack *singleParsePropertyStack,
                             ParsePropertyStack *propKeys,
                             ParsePropertyStack *propValues,
                             node_id *newNodeID) {
    if (depthStack->len >= MAX_NODE_DEPTH) {
        PRINT_ERROR("Max document node depth %u reached.\n", MAX_NODE_DEPTH);
        PRINT_ERROR("At tag %s.\n", tagStart);
        return DOCUMENT_TOO_DEEP;
    }

    DocumentStatus documentStatus = addToDocument(
        tagStart, tagLength, doc, previousNodeID, depthStack, 1,
        singleParsePropertyStack, propKeys, propValues, newNodeID);
    if (documentStatus != DOCUMENT_SUCCESS) {
        return documentStatus;
    }

    depthStack->stack[depthStack->len] = *newNodeID;
    depthStack->len++;

    return DOCUMENT_SUCCESS;
}

DocumentStatus putPropertyOnStack(size_t *currentStackLen,
                                  ParseProperty stack[MAX_PROPERTIES],
                                  const size_t propStart, const size_t propEnd,
                                  const char *xmlString) {
    const size_t propLen = propEnd - propStart;
    const char *start = &xmlString[propStart];
    if (*currentStackLen >= MAX_PROPERTIES) {
        PRINT_ERROR("Max number of %u properties per tag reached.\n",
                    MAX_PROPERTIES);
        ParseProperty attr = stack[*currentStackLen - 1];

        char buffer[propLen];
        strncpy(buffer, start, propLen);
        buffer[propLen] = '\0';
        PRINT_ERROR("Failed at parse property: \"%s\".\n", buffer);
        return DOCUMENT_TOO_MANY_ATTRIBUTES;
    }

    ParseProperty *attr = &stack[*currentStackLen];
    attr->start = start;
    attr->len = propLen;
    (*currentStackLen)++;

    return DOCUMENT_SUCCESS;
}

// DocumentStatus putParsePropertyValueOnStack(ParsePropertyValueStack
// *propValues,
//                                         const size_t start,
//                                         const size_t attributeEnd,
//                                         const size_t valueStart,
//                                         const size_t valueEnd) {
//     if (propValues->len >= MAX_PROPERTIES) {
//         PRINT_ERROR("Max number of %u attribute-values per tag reached.\n",
//                     MAX_PROPERTIES);
//         ParsePropertyValue attr =
//             propValues->stack[propValues->len - 1];
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
//    propValues->stack[propValues->len].len =
//        currentPosition - start + 1;
//    const char *startPos =
//        propValues->stack[propValues->len].start;
//    size_t singleLen =
//        propValues->stack[propValues->len].attribute;
//    propValues->len++;
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

    ParsePropertyStack binaryProps;
    binaryProps.len = 0;

    ParsePropertyStack propKeys;
    propKeys.len = 0;
    ParsePropertyStack propValues;
    propValues.len = 0;

    size_t propKeyStart = 0;
    size_t propValueStart = 0;

    DocumentStatus documentStatus = DOCUMENT_SUCCESS;

    node_id newNodeID = 0;
    node_id previousNodeID = 0;
    char ch = xmlString[currentPosition];
    while (ch != '\0') {
        // printf("Current state: %s\n", stateToString(state));
        // printf("Current char: %c\n", ch);

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
                documentStatus =
                    addPairedNode(&xmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &depthStack, &binaryProps,
                                  &propKeys, &propValues, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTRS:
            if (isAlphaBetical(ch)) {
                propKeyStart = currentPosition;
                state = ATTR_KEY;
            }
            if (ch == '/' || (isExclam && ch == '>')) {
                documentStatus =
                    addToDocument(&xmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &depthStack, 0, &binaryProps,
                                  &propKeys, &propValues, &newNodeID);
                isExclam = 0;
                state = FREE;
            } else if (ch == '>') {
                documentStatus =
                    addPairedNode(&xmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &depthStack, &binaryProps,
                                  &propKeys, &propValues, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTR_KEY: {
            if (ch == ' ' || ch == '>') {
                documentStatus = putPropertyOnStack(
                    &binaryProps.len, binaryProps.stack, propKeyStart,
                    currentPosition, xmlString);
                if (ch == ' ') {
                    state = ATTRS;
                } else {
                    if (documentStatus == DOCUMENT_SUCCESS) {
                        if (isExclam) {
                            documentStatus = addToDocument(
                                &xmlString[tagNameStart], tagLength, doc,
                                &previousNodeID, &depthStack, 0, &binaryProps,
                                &propKeys, &propValues, &newNodeID);
                            isExclam = 0;
                            state = FREE;
                        } else {
                            documentStatus = addPairedNode(
                                &xmlString[tagNameStart], tagLength, doc,
                                &previousNodeID, &depthStack, &binaryProps,
                                &propKeys, &propValues, &newNodeID);
                            state = OPEN_PAIRED;
                        }
                    }
                }
            } else if (ch == '=') {
                documentStatus = putPropertyOnStack(
                    &propKeys.len, propKeys.stack, propKeyStart,
                    currentPosition, xmlString);
                currentPosition += 2; // skip '="'
                propValueStart = currentPosition;
                state = ATTR_VALUE;
            }
            break;
        }
        case ATTR_VALUE: {
            if (ch == '"') {
                documentStatus = putPropertyOnStack(
                    &propValues.len, propValues.stack, propValueStart,
                    currentPosition, xmlString);
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
