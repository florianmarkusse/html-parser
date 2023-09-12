#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/utils/memory/memory.h"
#include "flo/html-parser/utils/text/text.h"

/**
 * Css queries are built up of 2 parts:
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

typedef enum { NORMAL, CLASS, ID, NUM_SELECTORS } Selector;

#define CHECK_FILTERS_LIMIT(filtersLen)                                        \
    do {                                                                       \
        if ((filtersLen) >= MAX_FILTERS_PER_ELEMENT) {                         \
            PRINT_ERROR("Too many filters in a single element detected!\n");   \
            return QUERY_TOO_MANY_ELEMENT_FILTERS;                             \
        }                                                                      \
    } while (0)

bool isPropStartChar(const char ch) { return isAlphaBetical(ch) || ch == '!'; }

bool isElementStartChar(const char ch) {
    return isPropStartChar(ch) || ch == '.' || ch == '#';
}

bool isSpecifiedCombinator(const char ch) {
    return ch == '>' || ch == '+' || ch == '~';
}

bool isCombinator(const char ch) {
    return ch == ' ' || isSpecifiedCombinator(ch);
}

bool endOfCurrentFilter(const char ch) {
    return isCombinator(ch) || isSpecialSpace(ch) || ch == '[' || ch == '.' ||
           ch == '#';
}

QueryStatus getQueryResults(const char *cssQuery, const Dom *dom,
                            const TextStore *textStore,
                            Uint16HashSet *set) {
    QueryStatus result = QUERY_SUCCESS;

    FilterType filters[MAX_FILTERS_PER_ELEMENT];
    size_t filtersLen = 0;

    Combinator currentCombinator = NO_COMBINATOR;
    Selector currentSelector = NORMAL;

    size_t tokenStart = 0;
    size_t tokenLength = 0;
    element_id tokenID = 0;

    size_t currentPosition = 0;
    char ch = cssQuery[currentPosition];

    // Skip ahead until an element is found.
    while (!isElementStartChar(ch) && ch != '[' && ch != '*' && ch != '\0') {
        ch = cssQuery[++currentPosition];
    }

    while (ch != '\0') {
        ch = cssQuery[currentPosition];

        if (isPropStartChar(ch)) {
            tokenStart = currentPosition;
            while (!endOfCurrentFilter(ch) && ch != '\0') {
                ch = cssQuery[++currentPosition];
            }
            tokenLength =
                currentPosition - tokenStart + 1; // Add 1 for the holy spirit.

            char buffer[tokenLength];
            strncpy(buffer, &cssQuery[tokenStart], tokenLength);
            buffer[tokenLength - 1] = '\0';

            tokenID = getTagID(buffer, textStore);
            if (tokenID == 0) {
                return QUERY_NOT_SEEN_BEFORE;
            }

            CHECK_FILTERS_LIMIT(filtersLen);
            filters[filtersLen].attributeSelector = TAG;
            filters[filtersLen].data.tagID = tokenID;
            filtersLen++;
        } else if (ch == '*') {
            while (!endOfCurrentFilter(ch) && ch != '\0') {
                ch = cssQuery[++currentPosition];
            }

            CHECK_FILTERS_LIMIT(filtersLen);
            filters[filtersLen].attributeSelector = ALL_NODES;
            filtersLen++;
        }

        while (ch == '[' || ch == '.' || ch == '#') {
            switch (ch) {
            case '.': {
                currentSelector = CLASS;
                break;
            }
            case '#': {
                currentSelector = ID;
                break;
            }
            case '[': {
                currentSelector = NORMAL;
                break;
            }
            default: {
                PRINT_ERROR("Unable to select selector!\n");
                currentSelector = NORMAL;
                break;
            }
            }

            while (!isPropStartChar(ch) && ch != '\0') {
                ch = cssQuery[++currentPosition];
            }

            tokenStart = currentPosition;
            while (ch != ' ' && !isSpecialSpace(ch) && ch != '=' && ch != ']' &&
                   ch != '\0') {
                ch = cssQuery[++currentPosition];
            }
            tokenLength =
                currentPosition - tokenStart + 1; // Add 1 for the holy spirit.

            while (ch != '=' && ch != ']' && ch != '.' && ch != '#' &&
                   ch != '\0') {
                ch = cssQuery[++currentPosition];
            }

            // If ch == '.' or ch == '#', it means we had to have had '.' or '#'
            // at the beginnging. This is a bit of a funky inference to make,
            // but it is correct (I think).
            if (currentSelector == CLASS || currentSelector == ID) {
                const char *keyBuffer =
                    currentSelector == CLASS ? "class" : "id";
                tokenID = getPropKeyID(keyBuffer, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                CHECK_FILTERS_LIMIT(filtersLen);
                filters[filtersLen].attributeSelector = PROPERTY;
                filters[filtersLen].data.keyValuePair.keyID = tokenID;

                char valueBuffer[tokenLength];
                strncpy(valueBuffer, &cssQuery[tokenStart], tokenLength);
                valueBuffer[tokenLength - 1] = '\0';

                tokenID = getPropValueID(valueBuffer, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                filters[filtersLen].data.keyValuePair.valueID = tokenID;
                filtersLen++;
            } else if (ch == ']') {
                char boolBuffer[tokenLength];
                strncpy(boolBuffer, &cssQuery[tokenStart], tokenLength);
                boolBuffer[tokenLength - 1] = '\0';

                tokenID = getBoolPropID(boolBuffer, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                CHECK_FILTERS_LIMIT(filtersLen);
                filters[filtersLen].attributeSelector = BOOLEAN_PROPERTY;
                filters[filtersLen].data.propID = tokenID;
                filtersLen++;
            } else if (ch == '=') {
                char keyBuffer[tokenLength];
                strncpy(keyBuffer, &cssQuery[tokenStart], tokenLength);
                keyBuffer[tokenLength - 1] = '\0';

                tokenID = getPropKeyID(keyBuffer, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }
                // Adding to filter already because I want to reuse tokenID
                // :)
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

                tokenID = getPropValueID(valueBuffer, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
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
            if ((result = getNodesWithoutCombinator(filters, filtersLen, dom,
                                                    set)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case ADJACENT: {
            if ((result = getFilteredAdjacents(filters, filtersLen, dom, 1,

                                               set)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case CHILD: {
            if ((result = getFilteredDescendants(filters, filtersLen, dom, 1,
                                                 set)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case GENERAL_SIBLING: {
            if ((result = getFilteredAdjacents(filters, filtersLen, dom,
                                               SIZE_MAX, set)) !=
                QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case DESCENDANT: {
            if ((result = getFilteredDescendants(filters, filtersLen, dom,
                                                 SIZE_MAX, set)) !=
                QUERY_SUCCESS) {
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

QueryStatus querySelectorAll(const char *cssQuery, const Dom *dom,
                             const TextStore *textStore,
                             node_id **results, size_t *resultsLen) {
    Uint16HashSet resultsSet;
    if (initUint16HashSet(&resultsSet, INITIAL_QUERY_CAP) != HASH_SUCCESS) {
        PRINT_ERROR(
            "Failed to allocate memory initializating querySelectorAll\n");
        return QUERY_MEMORY_ERROR;
    }

    QueryStatus result = QUERY_SUCCESS;

    char *comma = strchr(cssQuery, ',');

    if (comma != NULL) {
        char *queryCopy = strdup(cssQuery);
        if (queryCopy == NULL) {
            PRINT_ERROR("Failed to copy query for tokenizing\n");
            destroyUint16HashSet(&resultsSet);
            return QUERY_MEMORY_ERROR;
        }

        Uint16HashSet set;
        if (initUint16HashSet(&set, INITIAL_QUERY_CAP) != HASH_SUCCESS) {
            FREE_TO_NULL(queryCopy);
            PRINT_ERROR(
                "Failed to allocate memory initializating querySelectorAll\n");
            return QUERY_MEMORY_ERROR;
        }

        char *toFree = queryCopy;
        char *rest = NULL;
        char *token = NULL;
        while (token = strtok_r(queryCopy, ",", &rest)) {
            if ((result = getQueryResults(token, dom, textStore, &set)) !=
                QUERY_SUCCESS) {
                destroyUint16HashSet(&resultsSet);
                destroyUint16HashSet(&set);
                FREE_TO_NULL(toFree);
                ERROR_WITH_CODE_ONLY(queryingStatusToString(result),
                                     "Unable get query results!\n");
                return result;
            }

            Uint16HashSetIterator iterator;
            initUint16HashSetIterator(&iterator, &set);
            while (hasNextUint16HashSetIterator(&iterator)) {
                HashStatus insertStatus = insertUint16HashSet(
                    &resultsSet, nextUint16HashSetIterator(&iterator));
                if (insertStatus != HASH_SUCCESS) {
                    destroyUint16HashSet(&resultsSet);
                    destroyUint16HashSet(&set);
                    FREE_TO_NULL(toFree);
                    ERROR_WITH_CODE_ONLY(
                        hashStatusToString(insertStatus),
                        "Failed to save intermediate results!\n");

                    return QUERY_MEMORY_ERROR;
                }
            }

            resetUint16HashSet(&set);
            queryCopy = rest;
        }

        FREE_TO_NULL(toFree);
        destroyUint16HashSet(&set);
    } else {
        if ((result = getQueryResults(cssQuery, dom, textStore,
                                      &resultsSet)) != QUERY_SUCCESS) {
            destroyUint16HashSet(&resultsSet);
            ERROR_WITH_CODE_ONLY(queryingStatusToString(result),
                                 "Unable get query results!\n");
            return result;
        }
    }

    HashStatus conversionResult =
        uint16HashSetToArray(&resultsSet, results, resultsLen);
    if (conversionResult != HASH_SUCCESS) {
        ERROR_WITH_CODE_ONLY(hashStatusToString(conversionResult),
                             "Failed to convert set to array!\n");
        destroyUint16HashSet(&resultsSet);
        return QUERY_MEMORY_ERROR;
    }

    destroyUint16HashSet(&resultsSet);
    return result;
}

QueryStatus getElementsByClassName(const char *class, const Dom *dom,
                                   const TextStore *textStore,
                                   node_id **results, size_t *resultsLen) {
    size_t cssQueryLen = strlen(class) + 2;
    char cssQuery[cssQueryLen];
    snprintf(cssQuery, cssQueryLen, ".%s", class);

    return querySelectorAll(cssQuery, dom, textStore, results, resultsLen);
}

QueryStatus getElementsByTagName(const char *tag, const Dom *dom,
                                 const TextStore *textStore,
                                 node_id **results, size_t *resultsLen) {
    return querySelectorAll(tag, dom, textStore, results, resultsLen);
}

QueryStatus querySelector(const char *cssQuery, const Dom *dom,
                          const TextStore *textStore, node_id *result) {
    node_id *results = NULL;
    size_t resultsLen = 0;

    QueryStatus status =
        querySelectorAll(cssQuery, dom, textStore, &results, &resultsLen);
    if (status != QUERY_SUCCESS) {
        FREE_TO_NULL(results);
        return status;
    }

    if (resultsLen == 0) {
        FREE_TO_NULL(results);
        *result = 0;
        return QUERY_SUCCESS;
    }

    node_id currentNode = dom->firstNodeID;
    while (currentNode) {
        for (size_t i = 0; i < resultsLen; i++) {
            if (results[i] == currentNode) {
                FREE_TO_NULL(results);
                *result = currentNode;
                return QUERY_SUCCESS;
            }
        }
        currentNode = traverseDom(currentNode, dom);
    }
    FREE_TO_NULL(results);
    return status;
}

QueryStatus getElementByID(const char *id, const Dom *dom,
                           const TextStore *textStore,
                           node_id *result) {
    size_t cssQueryLen = strlen(id) + 2;
    char cssQuery[cssQueryLen];
    snprintf(cssQuery, cssQueryLen, "#%s", id);

    return querySelector(cssQuery, dom, textStore, result);
}
