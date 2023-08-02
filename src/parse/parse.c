#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dom/document-utils.h"
#include "dom/document.h"
#include "parse/parse-property.h"
#include "parse/parse.h"
#include "utils/print/error.h"
#include "utils/text/text.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

typedef enum {
    OPEN_TAG,
    CLOSE_TAG,
    TAG_NAME,
    ATTRS,
    ATTR_KEY,
    ATTR_VALUE,
    OPEN_PAIRED,
    TEXT_NODE,
    COMMENT,
    NUM_STATES
} State;

static inline const char *stateToString(State state) {
    static const char *stateStrings[NUM_STATES] = {
        "OPEN_TAG",   "CLOSE_TAG",   "TAG_NAME",  "ATTRS",  "ATTR_KEY",
        "ATTR_VALUE", "OPEN_PAIRED", "TEXT_NODE", "COMMENT"};

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

DocumentStatus updateReferences(const node_id newNodeID,
                                node_id *previousNodeID, NodeDepth *depthStack,
                                Document *doc) {
    if (newNodeID > 0 && *previousNodeID > 0) {
        if (depthStack->len == 0) {
            if (addNextNode(*previousNodeID, newNodeID, doc) !=
                DOCUMENT_SUCCESS) {
                return DOCUMENT_NO_ADD;
            }
        } else {
            const unsigned int parentNodeID =
                depthStack->stack[depthStack->len - 1];
            if (addParentChild(parentNodeID, newNodeID, doc) !=
                DOCUMENT_SUCCESS) {
                return DOCUMENT_NO_ADD;
            }
            if (parentNodeID == *previousNodeID) {
                if (addParentFirstChild(parentNodeID, newNodeID, doc) !=
                    DOCUMENT_SUCCESS) {
                    return DOCUMENT_NO_ADD;
                }
            } else {
                if (addNextNode(*previousNodeID, newNodeID, doc) !=
                    DOCUMENT_SUCCESS) {
                    return DOCUMENT_NO_ADD;
                }
            }
        }
    }
    *previousNodeID = newNodeID;

    return DOCUMENT_SUCCESS;
}

DocumentStatus addTextToDocument(const char *tagStart, const size_t tagLength,
                                 Document *doc, node_id *previousNodeID,
                                 NodeDepth *depthStack

) {
    // Comments require us to merge this and the previous text node :(
    Node prevNode = doc->nodes[*previousNodeID];
    if (isText(prevNode.tagID)) {
        const char *prevText = getText(prevNode.nodeID, doc);
        const size_t mergedLen =
            strlen(prevText) + tagLength + 2; // Adding a whitespace in between.

        char buffer[mergedLen];
        strcpy(buffer, prevText);
        strcat(buffer, " ");
        strncat(buffer, tagStart, tagLength);
        buffer[mergedLen - 1] = '\0';

        element_id updatedTextID = 0;

        if (elementToIndex(&gText.container, &gText.len, buffer, mergedLen, 1,
                           0, &updatedTextID) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }

        return replaceTextNode(prevNode.nodeID, updatedTextID, doc);
    }

    element_id tagID = 0;
    if (textElementToIndex(&tagID) != ELEMENT_SUCCESS) {
        return DOCUMENT_NO_ELEMENT;
    }
    node_id newNodeID = 0;
    if (addNode(&newNodeID, tagID, doc) != DOCUMENT_SUCCESS) {
        return DOCUMENT_NO_ADD;
    }

    element_id textID = 0;

    if (elementToIndex(&gText.container, &gText.len, tagStart, tagLength, 1, 0,
                       &textID) != ELEMENT_SUCCESS) {
        return DOCUMENT_NO_ELEMENT;
    }

    if (addTextNode(newNodeID, textID, doc) != DOCUMENT_SUCCESS) {
        return DOCUMENT_NO_ADD;
    }

    return updateReferences(newNodeID, previousNodeID, depthStack, doc);
}

DocumentStatus
addToDocument(const char *tagStart, size_t tagLength, Document *doc,
              node_id *previousNodeID, NodeDepth *depthStack,
              const unsigned char isPaired, ParsePropertyStack *binaryProps,
              ParsePropertyStack *propKeys, ParsePropertyStack *propValues,
              node_id *newNodeID) {
    element_id tagID = 0;
    if (elementToIndex(&gTags.container,
                       isPaired ? &gTags.pairedLen : &gTags.singleLen, tagStart,
                       tagLength, isPaired, 1, &tagID) != ELEMENT_SUCCESS) {
        return DOCUMENT_NO_ELEMENT;
    }
    if (addNode(newNodeID, tagID, doc) != DOCUMENT_SUCCESS) {
        return DOCUMENT_NO_ADD;
    }

    for (size_t i = 0; i < binaryProps->len; i++) {
        ParseProperty parseProp = binaryProps->stack[i];

        element_id propID = 0;

        if (elementToIndex(&gPropKeys.container, &gPropKeys.singleLen,
                           parseProp.start, parseProp.len, 0, 1,
                           &propID) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
        if (addBooleanProperty(*newNodeID, propID, doc) != DOCUMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
    }
    binaryProps->len = 0;

    for (size_t i = 0; i < propKeys->len; i++) {
        ParseProperty key = propKeys->stack[i];
        element_id keyID = 0;
        if (elementToIndex(&gPropKeys.container, &gPropKeys.pairedLen,
                           key.start, key.len, 1, 1,
                           &keyID) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }

        ParseProperty value = propValues->stack[i];
        element_id valueID = 0;
        if (elementToIndex(&gPropValues.container, &gPropValues.len,
                           value.start, value.len, 1, 1,
                           &valueID) != ELEMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }

        if (addProperty(*newNodeID, keyID, valueID, doc) != DOCUMENT_SUCCESS) {
            return DOCUMENT_NO_ELEMENT;
        }
    }
    propKeys->len = 0;
    propValues->len = 0;

    return updateReferences(*newNodeID, previousNodeID, depthStack, doc);
}

DocumentStatus
addPairedNode(const char *tagStart, size_t tagLength, Document *doc,
              node_id *previousNodeID, NodeDepth *depthStack,
              ParsePropertyStack *boolProps, ParsePropertyStack *propKeys,
              ParsePropertyStack *propValues, node_id *newNodeID) {
    if (depthStack->len >= MAX_NODE_DEPTH) {
        PRINT_ERROR("Max document node depth %u reached.\n", MAX_NODE_DEPTH);
        PRINT_ERROR("At tag %s.\n", tagStart);
        return DOCUMENT_TOO_DEEP;
    }

    DocumentStatus documentStatus =
        addToDocument(tagStart, tagLength, doc, previousNodeID, depthStack, 1,
                      boolProps, propKeys, propValues, newNodeID);
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
                                  const char *htmlString) {
    const size_t propLen = propEnd - propStart;
    const char *start = &htmlString[propStart];
    if (*currentStackLen >= MAX_PROPERTIES) {
        PRINT_ERROR("Max number of %u properties per tag reached.\n",
                    MAX_PROPERTIES);
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

DocumentStatus parse(const char *htmlString, Document *doc) {
    State state = OPEN_PAIRED;

    size_t currentPosition = 0;

    size_t textNodeStart = 0;
    unsigned char isNewline = 0;

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
    char ch = htmlString[currentPosition];
    while (ch != '\0') {
        printf("Current state: %s\n", stateToString(state));
        if (isprint(ch)) {
            printf("'%c' = %d\n", ch, ch);
        } else {
            switch (ch) {
            case '\0':
                printf("'\\0' (null terminator) = %d\n", ch);
                break;
            case '\a':
                printf("'\\a' (alert) = %d\n", ch);
                break;
            case '\b':
                printf("'\\b' (backspace) = %d\n", ch);
                break;
            case '\f':
                printf("'\\f' (form feed) = %d\n", ch);
                break;
            case '\r':
                printf("'\\r' (carriage return) = %d\n", ch);
                break;
            case '\t':
                printf("'\\t' (tab) = %d\n", ch);
                break;
            case '\v':
                printf("'\\v' (vertical tab) = %d\n", ch);
                break;
            case '\n':
                printf("'\\n' (newline) = %d\n", ch);
                break;
            case '\\':
                printf("'\\\\' (backslash) = %d\n", ch);
                break;
            case '\'':
                printf("'\\'' (single quote) = %d\n", ch);
                break;
            case '\"':
                printf("'\\\"' (double quote) = %d\n", ch);
                break;
            default:
                printf("'%c' (non-printable) = %d\n", ch, ch);
                break;
            }
        }

        switch (state) {
        case OPEN_TAG:
            if (ch == '/') {
                previousNodeID = depthStack.stack[depthStack.len - 1];
                depthStack.len--;
                state = CLOSE_TAG;
            }
            if (isAlphaBetical(ch)) {
                tagNameStart = currentPosition;
                state = TAG_NAME;
            }
            if (ch == '!') {
                if (htmlString[currentPosition + 1] == '-' &&
                    htmlString[currentPosition + 2] == '-') {
                    state = COMMENT;
                } else {
                    isExclam = 1;
                    tagNameStart = currentPosition;
                    state = TAG_NAME;
                }
            }
            break;
        case COMMENT: {
            if (ch == '>' && htmlString[MAX(0, currentPosition - 1)] == '-' &&
                htmlString[MAX(0, currentPosition - 2)] == '-') {
                state = OPEN_PAIRED;
            }
            break;
        }
        case TAG_NAME:
            if (ch == ' ') {
                tagLength = currentPosition - tagNameStart;
                state = ATTRS;
            }
            if (ch == '>') {
                tagLength = currentPosition - tagNameStart;
                documentStatus =
                    addPairedNode(&htmlString[tagNameStart], tagLength, doc,
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
                    addToDocument(&htmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &depthStack, 0, &binaryProps,
                                  &propKeys, &propValues, &newNodeID);
                isExclam = 0;
                if (ch == '/') {
                    state = CLOSE_TAG;
                } else {
                    state = OPEN_PAIRED;
                }
            } else if (ch == '>') {
                documentStatus =
                    addPairedNode(&htmlString[tagNameStart], tagLength, doc,
                                  &previousNodeID, &depthStack, &binaryProps,
                                  &propKeys, &propValues, &newNodeID);
                state = OPEN_PAIRED;
            }
            break;
        case ATTR_KEY: {
            if (ch == ' ' || ch == '>') {
                documentStatus = putPropertyOnStack(
                    &binaryProps.len, binaryProps.stack, propKeyStart,
                    currentPosition, htmlString);
                if (ch == ' ') {
                    state = ATTRS;
                } else {
                    if (documentStatus == DOCUMENT_SUCCESS) {
                        if (isExclam) {
                            documentStatus = addToDocument(
                                &htmlString[tagNameStart], tagLength, doc,
                                &previousNodeID, &depthStack, 0, &binaryProps,
                                &propKeys, &propValues, &newNodeID);
                            isExclam = 0;
                            state = OPEN_PAIRED;
                        } else {
                            documentStatus = addPairedNode(
                                &htmlString[tagNameStart], tagLength, doc,
                                &previousNodeID, &depthStack, &binaryProps,
                                &propKeys, &propValues, &newNodeID);
                            state = OPEN_PAIRED;
                        }
                    }
                }
            } else if (ch == '=') {
                documentStatus = putPropertyOnStack(
                    &propKeys.len, propKeys.stack, propKeyStart,
                    currentPosition, htmlString);
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
                    currentPosition, htmlString);
                state = ATTRS;
            }
            break;
        }
        case OPEN_PAIRED:
            if (!isNewline) {
                if (ch == '<') {
                    state = OPEN_TAG;
                } else if (ch == '\n') {
                    isNewline = 1;
                } else {
                    textNodeStart = currentPosition;
                    state = TEXT_NODE;
                }
            } else {
                if (ch == '<') {
                    isNewline = 0;
                    state = OPEN_TAG;
                } else if (ch != ' ' && ch != '\n' && ch != '\t') {
                    isNewline = 0;
                    textNodeStart = currentPosition;
                    state = TEXT_NODE;
                }
            }
            break;
        case TEXT_NODE:
            if (ch == '\n' || ch == '<') {
                size_t textNodeSize = currentPosition - textNodeStart;
                documentStatus =
                    addTextToDocument(&htmlString[textNodeStart], textNodeSize,
                                      doc, &previousNodeID, &depthStack);

                if (ch == '\n') {
                    isNewline = 1;
                    state = OPEN_PAIRED;
                } else {
                    state = OPEN_TAG;
                }
            }
            break;
        case CLOSE_TAG:
            if (ch == '>') {
                state = OPEN_PAIRED;
            }
            break;
        default:;
        }

        if (documentStatus != DOCUMENT_SUCCESS) {
            break;
        }

        ch = htmlString[++currentPosition];
    }

    return documentStatus;
}
