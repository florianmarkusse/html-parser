#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/query/dom-query-util.h"
#include "flo/html-parser/dom/query/dom-query.h"
#include "flo/html-parser/type/element/element-status.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/utils/memory/memory.h"
#include "flo/html-parser/utils/text/text.h"

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
        if ((filtersLen) >= MAX_FILTERS_PER_ELEMENT) {                         \
            PRINT_ERROR("Too many filters in a single element detected!\n");   \
            return QUERY_TOO_MANY_ELEMENT_FILTERS;                             \
        }                                                                      \
    } while (0)

bool isPropStartChar(char ch) { return isAlphaBetical(ch) || ch == '!'; }

bool isElementStartChar(char ch) {
    return isPropStartChar(ch) || ch == '.' || ch == '#';
}

bool isSpecifiedCombinator(char ch) {
    return ch == '>' || ch == '+' || ch == '~';
}

bool isCombinator(char ch) { return ch == ' ' || isSpecifiedCombinator(ch); }

QueryStatus querySelectorAll(const char *cssQuery, const Dom *dom,
                             const DataContainer *dataContainer,
                             node_id **results, size_t *resultsLen) {
    if (*results == NULL && *resultsLen == 0) {
        *results = malloc(sizeof(node_id) * INITIAL_QUERY_CAP);
        if (*results == NULL) {
            PRINT_ERROR("Failed to allocate memory at the outset\n");
            return QUERY_MEMORY_ERROR;
        }
    } else {
        PRINT_ERROR("The **results parameter must be pointing to NULL and its "
                    "corresponding *resultsLen pointing to 0\n");
        return QUERY_INITIALIZATION_ERROR;
    }

    QueryStatus result = QUERY_SUCCESS;

    size_t currentCap = INITIAL_QUERY_CAP;

    FilterType filters[MAX_FILTERS_PER_ELEMENT];
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
                QUERY_SUCCESS) {
                return result;
            }

            CHECK_FILTERS_LIMIT(filtersLen);
            filters[filtersLen].attributeSelector = TAG;
            filters[filtersLen].data.propID = tokenID;
            filtersLen++;
        }

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
                if ((result = getBoolPropID(boolBuffer, &tokenID,
                                            dataContainer)) != QUERY_SUCCESS) {
                    return result;
                }

                CHECK_FILTERS_LIMIT(filtersLen);
                filters[filtersLen].attributeSelector = BOOLEAN_PROPERTY;
                filters[filtersLen].data.propID = tokenID;
                filtersLen++;
            } else if (ch == '=') {
                char keyBuffer[tokenLength];
                strncpy(keyBuffer, &cssQuery[tokenStart], tokenLength);
                keyBuffer[tokenLength - 1] = '\0';

                tokenID = 0;
                if ((result = getKeyPropID(keyBuffer, &tokenID,
                                           dataContainer)) != QUERY_SUCCESS) {
                    return result;
                }
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
                                             dataContainer)) != QUERY_SUCCESS) {
                    return result;
                }

                filters[filtersLen].data.keyValuePair.valueID = tokenID;
                filtersLen++;
            }
            if (ch == ']') {
                ch = cssQuery[++currentPosition];
            }
        }

        if (filtersLen < 1) {
            PRINT_ERROR("Did not receive any filters in the css query\n");
            return QUERY_INVALID_ELEMENT;
        }

        // Do filtering :)
        switch (currentCombinator) {
        case NO_COMBINATOR: {
            if ((result = getNodesWithoutCombinator(
                     filters, filtersLen, dom, results, resultsLen,
                     &currentCap)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case ADJACENT: {
            if ((result = getFilteredAdjacents(filters, filtersLen, dom, 1,
                                               results, resultsLen,
                                               &currentCap)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case CHILD: {
            if ((result = getFilteredDescendants(
                     filters, filtersLen, dom, 1, results, resultsLen,
                     &currentCap)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case GENERAL_SIBLING: {
            if ((result = getFilteredAdjacents(filters, filtersLen, dom,
                                               SIZE_MAX, results, resultsLen,
                                               &currentCap)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case DESCENDANT: {
            if ((result = getFilteredDescendants(
                     filters, filtersLen, dom, SIZE_MAX, results, resultsLen,
                     &currentCap)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        default: {
            PRINT_ERROR("Unknown current combinator, aborting css query!\n");
            return QUERY_INVALID_COMBINATOR;
        }
        }

        filtersLen = 0;

        // Swoop up any possible next combinator and move on to a potentially
        // next element.
        char combinator = ' ';
        while (!isElementStartChar(ch) && ch != '[' && ch != '\0') {
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
            return QUERY_INVALID_COMBINATOR;
        }
        }
    }

    return result;
}
