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
 * A css query must start and end with an element. flo_html_Combinators connect
 * the elements with a relationship.
 */

typedef enum { NORMAL, CLASS, ID, NUM_SELECTORS } Selector;

#define CHECK_FILTERS_LIMIT(filtersLen)                                        \
    do {                                                                       \
        if ((filtersLen) >= FLO_HTML_MAX_FILTERS_PER_ELEMENT) {                \
            FLO_HTML_PRINT_ERROR("Too many filters in a single element detected!\n");   \
            return QUERY_TOO_MANY_ELEMENT_FILTERS;                             \
        }                                                                      \
    } while (0)

bool isPropStartChar(const char ch) { return flo_html_isAlphaBetical(ch) || ch == '!'; }

bool isElementStartChar(const char ch) {
    return isPropStartChar(ch) || ch == '.' || ch == '#';
}

bool isSpecifiedflo_html_Combinator(const char ch) {
    return ch == '>' || ch == '+' || ch == '~';
}

bool isflo_html_Combinator(const char ch) {
    return ch == ' ' || isSpecifiedflo_html_Combinator(ch);
}

bool endOfCurrentFilter(const char ch) {
    return isflo_html_Combinator(ch) || flo_html_isSpecialSpace(ch) || ch == '[' ||
           ch == '.' || ch == '#';
}

flo_html_QueryStatus getQueryResults(const char *cssQuery,
                                     const flo_html_Dom *dom,
                                     const flo_html_TextStore *textStore,
                                     flo_html_Uint16HashSet *set) {
    flo_html_QueryStatus result = QUERY_SUCCESS;

    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT];
    size_t filtersLen = 0;

    flo_html_Combinator currentflo_html_Combinator = NO_COMBINATOR;
    Selector currentSelector = NORMAL;

    size_t tokenStart = 0;
    size_t tokenLength = 0;
    flo_html_element_id tokenID = 0;

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

            tokenID = flo_html_getTagID(buffer, textStore);
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
                FLO_HTML_PRINT_ERROR("Unable to select selector!\n");
                currentSelector = NORMAL;
                break;
            }
            }

            while (!isPropStartChar(ch) && ch != '\0') {
                ch = cssQuery[++currentPosition];
            }

            tokenStart = currentPosition;
            while (ch != ' ' && !flo_html_isSpecialSpace(ch) && ch != '=' && ch != ']' &&
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
                tokenID = flo_html_getPropKeyID(keyBuffer, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                CHECK_FILTERS_LIMIT(filtersLen);
                filters[filtersLen].attributeSelector = PROPERTY;
                filters[filtersLen].data.keyValuePair.keyID = tokenID;

                char valueBuffer[tokenLength];
                strncpy(valueBuffer, &cssQuery[tokenStart], tokenLength);
                valueBuffer[tokenLength - 1] = '\0';

                tokenID = flo_html_getPropValueID(valueBuffer, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                filters[filtersLen].data.keyValuePair.valueID = tokenID;
                filtersLen++;
            } else if (ch == ']') {
                char boolBuffer[tokenLength];
                strncpy(boolBuffer, &cssQuery[tokenStart], tokenLength);
                boolBuffer[tokenLength - 1] = '\0';

                tokenID = flo_html_getBoolPropID(boolBuffer, textStore);
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

                tokenID = flo_html_getPropKeyID(keyBuffer, textStore);
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
                while (ch != ' ' && !flo_html_isSpecialSpace(ch) && ch != ']' &&
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

                tokenID = flo_html_getPropValueID(valueBuffer, textStore);
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
            FLO_HTML_PRINT_ERROR("Did not receive any filters in the css query\n");
            return QUERY_INVALID_ELEMENT;
        }

        // Do filtering :)
        switch (currentflo_html_Combinator) {
        case NO_COMBINATOR: {
            if ((result = flo_html_getNodesWithoutflo_html_Combinator(
                     filters, filtersLen, dom, set)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case ADJACENT: {
            if ((result =
                     flo_html_getFilteredAdjacents(filters, filtersLen, dom, 1,

                                                   set)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case CHILD: {
            if ((result = flo_html_getFilteredDescendants(
                     filters, filtersLen, dom, 1, set)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case GENERAL_SIBLING: {
            if ((result = flo_html_getFilteredAdjacents(filters, filtersLen,
                                                        dom, SIZE_MAX, set)) !=
                QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case DESCENDANT: {
            if ((result = flo_html_getFilteredDescendants(
                     filters, filtersLen, dom, SIZE_MAX, set)) !=
                QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        default: {
            FLO_HTML_PRINT_ERROR("Unknown current combinator, aborting css query!\n");
            return QUERY_INVALID_COMBINATOR;
        }
        }

        filtersLen = 0;

        // Swoop up any possible next combinator and move on to a potentially
        // next element.
        char combinator = ' ';
        while (!isElementStartChar(ch) && ch != '[' && ch != '\0') {
            if (isSpecifiedflo_html_Combinator(ch)) {
                combinator = ch;
            }
            ch = cssQuery[++currentPosition];
        }

        switch (combinator) {
        case ' ': {
            currentflo_html_Combinator = DESCENDANT;
            break;
        }
        case '+': {
            currentflo_html_Combinator = ADJACENT;
            break;
        }
        case '>': {
            currentflo_html_Combinator = CHILD;
            break;
        }
        case '~': {
            currentflo_html_Combinator = GENERAL_SIBLING;
            break;
        }
        default: {
            FLO_HTML_PRINT_ERROR("Could not determine combinator!\n");
            return QUERY_INVALID_COMBINATOR;
        }
        }
    }

    return result;
}

flo_html_QueryStatus
flo_html_querySelectorAll(const char *cssQuery, const flo_html_Dom *dom,
                          const flo_html_TextStore *textStore,
                          flo_html_node_id **results, size_t *resultsLen) {
    flo_html_Uint16HashSet resultsSet;
    if (flo_html_initUint16HashSet(&resultsSet, FLO_HTML_INITIAL_QUERY_CAP) !=
        HASH_SUCCESS) {
        FLO_HTML_PRINT_ERROR(
            "Failed to allocate memory initializating querySelectorAll\n");
        return QUERY_MEMORY_ERROR;
    }

    flo_html_QueryStatus result = QUERY_SUCCESS;

    char *comma = strchr(cssQuery, ',');

    if (comma != NULL) {
        char *queryCopy = strdup(cssQuery);
        if (queryCopy == NULL) {
            FLO_HTML_PRINT_ERROR("Failed to copy query for tokenizing\n");
            flo_html_destroyUint16HashSet(&resultsSet);
            return QUERY_MEMORY_ERROR;
        }

        flo_html_Uint16HashSet set;
        if (flo_html_initUint16HashSet(&set, FLO_HTML_INITIAL_QUERY_CAP) !=
            HASH_SUCCESS) {
            FLO_HTML_FREE_TO_NULL(queryCopy);
            FLO_HTML_PRINT_ERROR(
                "Failed to allocate memory initializating querySelectorAll\n");
            return QUERY_MEMORY_ERROR;
        }

        char *toFree = queryCopy;
        char *rest = NULL;
        char *token = NULL;
        while (token = strtok_r(queryCopy, ",", &rest)) {
            if ((result = getQueryResults(token, dom, textStore, &set)) !=
                QUERY_SUCCESS) {
                flo_html_destroyUint16HashSet(&resultsSet);
                flo_html_destroyUint16HashSet(&set);
                FLO_HTML_FREE_TO_NULL(toFree);
                FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_queryingStatusToString(result),
                                     "Unable get query results!\n");
                return result;
            }

            flo_html_Uint16HashSetIterator iterator;
            flo_html_initUint16HashSetIterator(&iterator, &set);
            while (flo_html_hasNextUint16HashSetIterator(&iterator)) {
                flo_html_HashStatus insertStatus = flo_html_insertUint16HashSet(
                    &resultsSet, flo_html_nextUint16HashSetIterator(&iterator));
                if (insertStatus != HASH_SUCCESS) {
                    flo_html_destroyUint16HashSet(&resultsSet);
                    flo_html_destroyUint16HashSet(&set);
                    FLO_HTML_FREE_TO_NULL(toFree);
                    FLO_HTML_ERROR_WITH_CODE_ONLY(
                        flo_html_hashStatusToString(insertStatus),
                        "Failed to save intermediate results!\n");

                    return QUERY_MEMORY_ERROR;
                }
            }

            flo_html_resetUint16HashSet(&set);
            queryCopy = rest;
        }

        FLO_HTML_FREE_TO_NULL(toFree);
        flo_html_destroyUint16HashSet(&set);
    } else {
        if ((result = getQueryResults(cssQuery, dom, textStore, &resultsSet)) !=
            QUERY_SUCCESS) {
            flo_html_destroyUint16HashSet(&resultsSet);
            FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_queryingStatusToString(result),
                                 "Unable get query results!\n");
            return result;
        }
    }

    flo_html_HashStatus conversionResult =
        flo_html_uint16HashSetToArray(&resultsSet, results, resultsLen);
    if (conversionResult != HASH_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_hashStatusToString(conversionResult),
                             "Failed to convert set to array!\n");
        flo_html_destroyUint16HashSet(&resultsSet);
        return QUERY_MEMORY_ERROR;
    }

    flo_html_destroyUint16HashSet(&resultsSet);
    return result;
}

flo_html_QueryStatus
flo_html_getElementsByClassName(const char *class, const flo_html_Dom *dom,
                                const flo_html_TextStore *textStore,
                                flo_html_node_id **results,
                                size_t *resultsLen) {
    size_t cssQueryLen = strlen(class) + 2;
    char cssQuery[cssQueryLen];
    snprintf(cssQuery, cssQueryLen, ".%s", class);

    return flo_html_querySelectorAll(cssQuery, dom, textStore, results,
                                     resultsLen);
}

flo_html_QueryStatus
flo_html_getElementsByTagName(const char *tag, const flo_html_Dom *dom,
                              const flo_html_TextStore *textStore,
                              flo_html_node_id **results, size_t *resultsLen) {
    return flo_html_querySelectorAll(tag, dom, textStore, results, resultsLen);
}

flo_html_QueryStatus flo_html_querySelector(const char *cssQuery,
                                            const flo_html_Dom *dom,
                                            const flo_html_TextStore *textStore,
                                            flo_html_node_id *result) {
    flo_html_node_id *results = NULL;
    size_t resultsLen = 0;

    flo_html_QueryStatus status = flo_html_querySelectorAll(
        cssQuery, dom, textStore, &results, &resultsLen);
    if (status != QUERY_SUCCESS) {
        FLO_HTML_FREE_TO_NULL(results);
        return status;
    }

    if (resultsLen == 0) {
        FLO_HTML_FREE_TO_NULL(results);
        *result = 0;
        return QUERY_SUCCESS;
    }

    flo_html_node_id currentNode = dom->firstNodeID;
    while (currentNode) {
        for (size_t i = 0; i < resultsLen; i++) {
            if (results[i] == currentNode) {
                FLO_HTML_FREE_TO_NULL(results);
                *result = currentNode;
                return QUERY_SUCCESS;
            }
        }
        currentNode = flo_html_traverseDom(currentNode, dom);
    }
    FLO_HTML_FREE_TO_NULL(results);
    return status;
}

flo_html_QueryStatus
flo_html_getElementByID(const char *id, const flo_html_Dom *dom,
                        const flo_html_TextStore *textStore,
                        flo_html_node_id *result) {
    size_t cssQueryLen = strlen(id) + 2;
    char cssQuery[cssQueryLen];
    snprintf(cssQuery, cssQueryLen, "#%s", id);

    return flo_html_querySelector(cssQuery, dom, textStore, result);
}
