#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "dom/querying/document-querying-util.h"
#include "dom/querying/document-querying.h"
#include "type/element/element-status.h"
#include "type/element/elements.h"
#include "utils/memory/memory.h"
#include "utils/text/text.h"

/**
 * Css queries are built up of 2 parts:
 * TODO: implemenet the universal selector.;
 * - elements: For example, "body", ".special-class", "#my-id", "!DOCTYPE", '*',
 * or an attribute selector such as: [required] or [type=text]. These can also
 * be strung together like body[required]. There can be at most 1 tag selector
 * present in an element. Moreover, if this tag selector is present, it is
 * always the first one, followed by any number of attribute selectors.
 * - combinators: For example, ' ', '>', '+', or '~'.
 *
 * A css query must start and end with an element. Combinators connect the
 * elements with a relationship.
 */

#define CHECK_FILTERS_LIMIT(filtersLen)                                        \
    do {                                                                       \
        if ((filtersLen) >= ATTRIBUTE_MAX_SELECTORS) {                         \
            PRINT_ERROR("Too many filters in a single element detected!\n");   \
            return QUERYING_TOO_MANY_ELEMENT_FILTERS;                          \
        }                                                                      \
    } while (0)

#define ATTRIBUTE_MAX_SELECTORS (1U << 3U)

bool isPropStartChar(char ch) { return isAlphaBetical(ch) || ch == '!'; }

bool isElementStartChar(char ch) {
    return isPropStartChar(ch) || ch == '.' || ch == '#';
}

bool isSpecifiedCombinator(char ch) {
    return ch == '>' || ch == '+' || ch == '~';
}

bool isCombinator(char ch) { return ch == ' ' || isSpecifiedCombinator(ch); }

QueryingStatus querySelectorAll(const char *cssQuery, const Document *doc,
                                const DataContainer *dataContainer) {
    QueryingStatus result = QUERYING_SUCCESS;

    node_id *results = NULL;
    size_t resultsLen = 0;
    size_t currentCap = INITIAL_QUERY_CAP;

    FilterType filters[ATTRIBUTE_MAX_SELECTORS];
    size_t filtersLen = 0;

    Combinator currentCombinator = NO_COMBINATOR;

    size_t tokenStart = 0;
    size_t tokenLength = 0;
    element_id tokenID = 0;

    size_t currentPosition = 0;
    char ch = cssQuery[currentPosition];

    // Skip ahead until an element is found.
    while (!isElementStartChar(ch) && ch != '[' && ch != '\0') {
        ch = cssQuery[++currentPosition];
    }

    while (ch != '\0') {
        ch = cssQuery[currentPosition];

        if (isElementStartChar(ch)) {
            tokenStart = currentPosition;
            while (!isCombinator(ch) && !isSpecialSpace(ch) && ch != '[' &&
                   ch != '\0') {
                ch = cssQuery[++currentPosition];
            }
            tokenLength =
                currentPosition - tokenStart + 1; // Add 1 for the holy spirit.

            char buffer[tokenLength];
            strncpy(buffer, &cssQuery[tokenStart], tokenLength);
            buffer[tokenLength - 1] = '\0';

            tokenID = 0;
            if ((result = getTagID(buffer, &tokenID, dataContainer)) !=
                QUERYING_SUCCESS) {
                return result;
            }
            printf("Found the following tag: %s with id: %u\n", buffer,
                   tokenID);

            CHECK_FILTERS_LIMIT(filtersLen);
            filters[filtersLen].attributeSelector = TAG;
            filters[filtersLen].data.propID = tokenID;
            filtersLen++;
        }

        printf("after element, char is %c\n", ch);

        while (ch == '[') {
            while (!isElementStartChar(ch) && ch != '\0') {
                ch = cssQuery[++currentPosition];
            }

            tokenStart = currentPosition;
            while (ch != ' ' && !isSpecialSpace(ch) && ch != '=' && ch != ']' &&
                   ch != '\0') {
                ch = cssQuery[++currentPosition];
            }
            tokenLength =
                currentPosition - tokenStart + 1; // Add 1 for the holy spirit.

            while (ch != '=' && ch != ']' && ch != '\0') {
                ch = cssQuery[++currentPosition];
            }

            if (ch == ']') {
                char boolBuffer[tokenLength];
                strncpy(boolBuffer, &cssQuery[tokenStart], tokenLength);
                boolBuffer[tokenLength - 1] = '\0';

                tokenID = 0;
                if ((result =
                         getBoolPropID(boolBuffer, &tokenID, dataContainer)) !=
                    QUERYING_SUCCESS) {
                    return result;
                }
                printf("Found the following bool prop: %s with id: %u\n",
                       boolBuffer, tokenID);

                CHECK_FILTERS_LIMIT(filtersLen);
                filters[filtersLen].attributeSelector = BOOLEAN_PROPERTY;
                filters[filtersLen].data.propID = tokenID;
                filtersLen++;
            } else if (ch == '=') {
                char keyBuffer[tokenLength];
                strncpy(keyBuffer, &cssQuery[tokenStart], tokenLength);
                keyBuffer[tokenLength - 1] = '\0';

                tokenID = 0;
                if ((result =
                         getKeyPropID(keyBuffer, &tokenID, dataContainer)) !=
                    QUERYING_SUCCESS) {
                    return result;
                }
                printf("Found the following key prop: %s with id: %u\n",
                       keyBuffer, tokenID);
                // Adding to filter already because I want to reuse tokenID :)
                CHECK_FILTERS_LIMIT(filtersLen);
                filters[filtersLen].attributeSelector = PROPERTY;
                filters[filtersLen].data.keyValuePair.keyID = tokenID;

                // Skip the '='
                ch = cssQuery[++currentPosition];

                while (!isElementStartChar(ch) && ch != '\0') {
                    ch = cssQuery[++currentPosition];
                }
                tokenStart = currentPosition;
                while (ch != ' ' && !isSpecialSpace(ch) && ch != ']' &&
                       ch != '\0') {
                    ch = cssQuery[++currentPosition];
                }
                tokenLength = currentPosition - tokenStart +
                              1; // Add 1 for the holy spirit.
                while (ch != ']' && ch != '\0') {
                    ch = cssQuery[++currentPosition];
                }

                char valueBuffer[tokenLength];
                strncpy(valueBuffer, &cssQuery[tokenStart], tokenLength);
                valueBuffer[tokenLength - 1] = '\0';

                tokenID = 0;
                if ((result = getValuePropID(valueBuffer, &tokenID,
                                             dataContainer)) !=
                    QUERYING_SUCCESS) {
                    return result;
                }
                printf("Found the following value prop: %s with token id: %u\n",
                       valueBuffer, tokenID);

                filters[filtersLen].data.keyValuePair.valueID = tokenID;
                filtersLen++;
            }
            if (ch == ']') {
                ch = cssQuery[++currentPosition];
            }
        }

        if (filtersLen < 1) {
            PRINT_ERROR("Did not receive any filters in the css query\n");
            return QUERYING_INVALID_ELEMENT;
        }

        // Do filtering :)
        // blabla
        printf("Current size of filters: %zu\n", filtersLen);

        for (int i = 0; i < filtersLen; i++) {
            printf("Filter type: %u\n", filters[i].attributeSelector);
            switch (filters[i].attributeSelector) {
            case TAG: {
                printf("Filter tag id: %u\n", filters[i].data.tagID);
                break;
            }
            case BOOLEAN_PROPERTY: {
                printf("Filter bool prop id: %u\n", filters[i].data.propID);
                break;
            }
            case PROPERTY: {
                printf("Filter key id: %u, value id %u\n",
                       filters[i].data.keyValuePair.keyID,
                       filters[i].data.keyValuePair.valueID);
                break;
            }
            }
        }

        filtersLen = 0;

        // Swoop up any possible next combinator and move on to a potentially
        // next element.
        char combinator = ' ';
        while (!isElementStartChar(ch) && ch != '\0') {
            if (isSpecifiedCombinator(ch)) {
                combinator = ch;
            }
            ch = cssQuery[++currentPosition];
        }

        switch (combinator) {
        case ' ': {
            currentCombinator = DESCENDANT;
            break;
        }
        case '+': {
            currentCombinator = ADJACENT;
            break;
        }
        case '>': {
            currentCombinator = CHILD;
            break;
        }
        case '~': {
            currentCombinator = GENERAL_SIBLING;
            break;
        }
        default: {
            PRINT_ERROR("Could not determine combinator!\n");
            return QUERYING_INVALID_COMBINATOR;
        }
        }
    }

    return result;
}
