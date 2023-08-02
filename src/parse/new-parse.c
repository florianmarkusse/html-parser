#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "dom/document-utils.h"
#include "dom/document.h"
#include "parse/parse-property.h"
#include "parse/parse.h"
#include "type/element/elements.h"
#include "utils/print/error.h"
#include "utils/text/text.h"

#define MAX(a, b) (((a) > (b)) ? (a) : (b))

unsigned char isSpecialSpace(char ch) {
    return ch == '\t' || ch == '\n' || ch == '\r';
}

DocumentStatus getNewNodeID(node_id *currentNodeID, node_id *prevNodeID,
                            Document *doc) {
    *prevNodeID = *currentNodeID;
    return createNode(currentNodeID, doc);
}

DocumentStatus parseNEW(const char *htmlString, Document *doc) {
    DocumentStatus documentStatus = DOCUMENT_SUCCESS;

    element_id textTagID = 0;
    if (textElementToIndex(&textTagID) != ELEMENT_SUCCESS) {
        PRINT_ERROR("Failed to initialize tag ID for text nodes\n");
        return DOCUMENT_NO_ELEMENT;
    }

    size_t currentPosition = 0;
    size_t elementStartIndex = 0;

    node_id prevNodeID = 0;
    node_id currentNodeID = 0;
    char ch = htmlString[currentPosition];

    while (ch != '\0') {
        while (ch == ' ' || isSpecialSpace(ch)) {
            ch = htmlString[++currentPosition];
        }
        if (ch == '\0') {
            break;
        }

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
        printf("current char %c with pos %zu\n", ch, currentPosition);

        // Text node.
        if (ch != '<') {
            elementStartIndex = currentPosition;
            // Continue until we encounter extra space or the end of the text
            // node.
            while (
                ch != '<' && ch != '\0' && !isSpecialSpace(ch) &&
                (ch != ' ' || htmlString[MAX(0, currentPosition - 1)] != ' ')) {
                ch = htmlString[++currentPosition];
            }

            size_t elementLen = currentPosition - elementStartIndex;
            if (ch == ' ') {
                elementLen--;
            }
            char buffer[100];
            strncpy(buffer, &htmlString[elementStartIndex], elementLen);
            buffer[elementLen] = '\0';
            printf("the buffer: %s\n", buffer);

            element_id textID = 0;

            Node prevNode = doc->nodes[currentNodeID];
            if (isText(prevNode.tagID)) {
                printf("MERGIN\n");
                const char *prevText = getText(prevNode.nodeID, doc);
                const size_t mergedLen = strlen(prevText) + elementLen +
                                         2; // Adding a whitespace in between.

                char buffer[mergedLen];
                strcpy(buffer, prevText);
                strcat(buffer, " ");
                strncat(buffer, &htmlString[elementStartIndex], elementLen);
                buffer[mergedLen - 1] = '\0';

                if (elementToIndex(&gText.container, &gText.len, buffer,
                                   mergedLen, 1, 0,
                                   &textID) != ELEMENT_SUCCESS) {
                    PRINT_ERROR(
                        "Failed to create text ID for merging text nodes.\n");
                    return DOCUMENT_NO_ELEMENT;
                }

                if ((documentStatus = replaceTextNode(
                         currentNodeID, textID, doc)) != DOCUMENT_SUCCESS) {
                    PRINT_ERROR(
                        "Failed to replace the text node for a merge.\n");
                    return documentStatus;
                }
            } else {
                printf("CREATING NEW\n");
                if ((documentStatus = getNewNodeID(&currentNodeID, &prevNodeID,
                                                   doc)) != DOCUMENT_SUCCESS) {
                    PRINT_ERROR("Failed to create node for document.\n");
                    return documentStatus;
                }

                if (elementToIndex(&gText.container, &gText.len,
                                   &htmlString[elementStartIndex], elementLen,
                                   1, 0, &textID) != ELEMENT_SUCCESS) {
                    PRINT_ERROR("Failed to create text ID.\n");
                    return DOCUMENT_NO_ELEMENT;
                }

                if ((documentStatus = addTextNode(currentNodeID, textID,
                                                  doc)) != DOCUMENT_SUCCESS) {
                    PRINT_ERROR("Failed to add text node to document.\n");
                    return DOCUMENT_NO_ADD;
                }

                if ((documentStatus = setTagID(currentNodeID, textTagID,
                                               doc)) != DOCUMENT_SUCCESS) {
                    PRINT_ERROR(
                        "Failed to set tag ID to text id to document.\n");
                    return DOCUMENT_NO_ADD;
                }
            }

            ch = htmlString[--currentPosition];
        }

        // kind of obvious for now...
        if (ch == '<') {
            // Comments or <!DOCTYPE>.
            if (htmlString[currentPosition + 1] == '!') {
                if (htmlString[currentPosition + 2] == '-' &&
                    htmlString[currentPosition + 3] == '-') {
                    while (ch != '\0' &&
                           (ch != '>' ||
                            htmlString[MAX(0, currentPosition - 1)] != '-' ||
                            htmlString[MAX(0, currentPosition - 2)] != '-')) {
                        ch = htmlString[++currentPosition];
                    }

                } else {
                }
            }

            // if ((documentStatus = createNode(&currentNodeID, doc)) !=
            //     DOCUMENT_SUCCESS) {
            //     PRINT_ERROR("Failed to create node.\n");
            //     return documentStatus;
            // }

            // size_t elementStartIndex = currentPosition;
            // while (ch != '<' && ch != '\0') {
            //     ch = htmlString[++currentPosition];
            // }
        }

        ch = htmlString[++currentPosition];
    }

    return DOCUMENT_SUCCESS;
}
