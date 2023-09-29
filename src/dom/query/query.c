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
            FLO_HTML_PRINT_ERROR(                                              \
                "Too many filters in a single element detected!\n");           \
            return QUERY_TOO_MANY_ELEMENT_FILTERS;                             \
        }                                                                      \
    } while (0)

bool isPropStartChar(const char ch) {
    return flo_html_isAlphaBetical(ch) || ch == '!';
}

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
    return isflo_html_Combinator(ch) || flo_html_isSpecialSpace(ch) ||
           ch == '[' || ch == '.' || ch == '#';
}

flo_html_QueryStatus getQueryResults(const flo_html_String cssQuery,
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
    char ch = flo_html_getChar(cssQuery, currentPosition);

    // Skip ahead until an element is found.
    while (!isElementStartChar(ch) && ch != '[' && ch != '*' && ch != '\0') {
        ch = flo_html_getChar(cssQuery, ++currentPosition);
    }

    while (ch != '\0') {
        ch = flo_html_getChar(cssQuery, currentPosition);

        if (isPropStartChar(ch)) {
            tokenStart = currentPosition;
            while (!endOfCurrentFilter(ch) && ch != '\0') {
                ch = flo_html_getChar(cssQuery, ++currentPosition);
            }
            tokenLength =
                currentPosition - tokenStart + 1; // Add 1 for the holy spirit.

            unsigned char buffer[tokenLength];
            flo_html_String token = {tokenLength, buffer};
            flo_html_strcpy(
                token, FLO_HTML_S_LEN(flo_html_getCharPtr(cssQuery, tokenStart),
                                      tokenLength));

            tokenID = flo_html_getTagID(token, textStore);
            if (tokenID == 0) {
                return QUERY_NOT_SEEN_BEFORE;
            }

            CHECK_FILTERS_LIMIT(filtersLen);
            filters[filtersLen].attributeSelector = TAG;
            filters[filtersLen].data.tagID = tokenID;
            filtersLen++;
        } else if (ch == '*') {
            while (!endOfCurrentFilter(ch) && ch != '\0') {
                ch = flo_html_getChar(cssQuery, ++currentPosition);
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
                ch = flo_html_getChar(cssQuery, ++currentPosition);
            }

            tokenStart = currentPosition;
            while (ch != ' ' && !flo_html_isSpecialSpace(ch) && ch != '=' &&
                   ch != ']' && ch != '\0') {
                ch = flo_html_getChar(cssQuery, ++currentPosition);
            }
            tokenLength =
                currentPosition - tokenStart + 1; // Add 1 for the holy spirit.

            while (ch != '=' && ch != ']' && ch != '.' && ch != '#' &&
                   ch != '\0') {
                ch = flo_html_getChar(cssQuery, ++currentPosition);
            }

            // If ch == '.' or ch == '#', it means we had to have had '.' or '#'
            // at the beginnging. This is a bit of a funky inference to make,
            // but it is correct (I think).
            if (currentSelector == CLASS || currentSelector == ID) {
                const flo_html_String keyBuffer = currentSelector == CLASS
                                                      ? FLO_HTML_S("class")
                                                      : FLO_HTML_S("id");
                tokenID = flo_html_getPropKeyID(keyBuffer, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                CHECK_FILTERS_LIMIT(filtersLen);
                filters[filtersLen].attributeSelector = PROPERTY;
                filters[filtersLen].data.keyValuePair.keyID = tokenID;

                unsigned char valueBuffer[tokenLength];
                flo_html_String token = {tokenLength, valueBuffer};
                flo_html_strcpy(token, FLO_HTML_S_LEN(flo_html_getCharPtr(
                                                          cssQuery, tokenStart),
                                                      tokenLength));

                tokenID = flo_html_getPropValueID(token, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                filters[filtersLen].data.keyValuePair.valueID = tokenID;
                filtersLen++;
            } else if (ch == ']') {
                unsigned char boolBuffer[tokenLength];
                flo_html_String token = {tokenLength, boolBuffer};
                flo_html_strcpy(token, FLO_HTML_S_LEN(flo_html_getCharPtr(
                                                          cssQuery, tokenStart),
                                                      tokenLength));

                tokenID = flo_html_getBoolPropID(token, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                CHECK_FILTERS_LIMIT(filtersLen);
                filters[filtersLen].attributeSelector = BOOLEAN_PROPERTY;
                filters[filtersLen].data.propID = tokenID;
                filtersLen++;
            } else if (ch == '=') {
                unsigned char keyBuffer[tokenLength];

                flo_html_String keyToken = {tokenLength, keyBuffer};
                flo_html_strcpy(
                    keyToken,
                    FLO_HTML_S_LEN(flo_html_getCharPtr(cssQuery, tokenStart),
                                   tokenLength));

                tokenID = flo_html_getPropKeyID(keyToken, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }
                // Adding to filter already because I want to reuse tokenID
                // :)
                CHECK_FILTERS_LIMIT(filtersLen);
                filters[filtersLen].attributeSelector = PROPERTY;
                filters[filtersLen].data.keyValuePair.keyID = tokenID;

                // Skip the '='
                ch = flo_html_getChar(cssQuery, ++currentPosition);

                while (!isElementStartChar(ch) && ch != '\0') {
                    ch = flo_html_getChar(cssQuery, ++currentPosition);
                }
                tokenStart = currentPosition;
                while (ch != ' ' && !flo_html_isSpecialSpace(ch) && ch != ']' &&
                       ch != '\0') {
                    ch = flo_html_getChar(cssQuery, ++currentPosition);
                }
                tokenLength = currentPosition - tokenStart +
                              1; // Add 1 for the holy spirit.
                while (ch != ']' && ch != '\0') {
                    ch = flo_html_getChar(cssQuery, ++currentPosition);
                }

                unsigned char valueBuffer[tokenLength];

                flo_html_String valueToken = {tokenLength, valueBuffer};
                flo_html_strcpy(
                    valueToken,
                    FLO_HTML_S_LEN(flo_html_getCharPtr(cssQuery, tokenStart),
                                   tokenLength));

                tokenID = flo_html_getPropValueID(valueToken, textStore);
                if (tokenID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                filters[filtersLen].data.keyValuePair.valueID = tokenID;
                filtersLen++;
            }
            if (ch == ']') {
                ch = flo_html_getChar(cssQuery, ++currentPosition);
            }
        }

        if (filtersLen < 1) {
            FLO_HTML_PRINT_ERROR(
                "Did not receive any filters in the css query\n");
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
            FLO_HTML_PRINT_ERROR(
                "Unknown current combinator, aborting css query!\n");
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
            ch = flo_html_getChar(cssQuery, ++currentPosition);
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
flo_html_querySelectorAll(const flo_html_String cssQuery,
                          const flo_html_Dom *dom,
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

    if (flo_html_containsChar(cssQuery, ',')) {
        flo_html_Uint16HashSet set;
        if (flo_html_initUint16HashSet(&set, FLO_HTML_INITIAL_QUERY_CAP) !=
            HASH_SUCCESS) {
            FLO_HTML_PRINT_ERROR(
                "Failed to allocate memory initializating querySelectorAll\n");
            return QUERY_MEMORY_ERROR;
        }

        ptrdiff_t currentIndex = 0;
        ptrdiff_t commaIndex =
            flo_html_firstOccurenceOfFrom(cssQuery, ',', currentIndex);
        currentIndex = commaIndex;
        while (commaIndex > 0) {
            flo_html_String singularQuery =
                FLO_HTML_S_LEN(cssQuery.buf + commaIndex, commaIndex);

            if ((result = getQueryResults(singularQuery, dom, textStore,
                                          &set)) != QUERY_SUCCESS) {
                flo_html_destroyUint16HashSet(&resultsSet);
                flo_html_destroyUint16HashSet(&set);
                FLO_HTML_ERROR_WITH_CODE_ONLY(
                    flo_html_queryingStatusToString(result),
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
                    FLO_HTML_ERROR_WITH_CODE_ONLY(
                        flo_html_hashStatusToString(insertStatus),
                        "Failed to save intermediate results!\n");

                    return QUERY_MEMORY_ERROR;
                }
            }

            flo_html_resetUint16HashSet(&set);

            commaIndex =
                flo_html_firstOccurenceOfFrom(cssQuery, ',', currentIndex);
            currentIndex = commaIndex;
        }

        flo_html_destroyUint16HashSet(&set);
    } else {
        if ((result = getQueryResults(cssQuery, dom, textStore, &resultsSet)) !=
            QUERY_SUCCESS) {
            flo_html_destroyUint16HashSet(&resultsSet);
            FLO_HTML_ERROR_WITH_CODE_ONLY(
                flo_html_queryingStatusToString(result),
                "Unable get query results!\n");
            return result;
        }
    }

    flo_html_HashStatus conversionResult =
        flo_html_uint16HashSetToArray(&resultsSet, results, resultsLen);
    if (conversionResult != HASH_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_hashStatusToString(conversionResult),
            "Failed to convert set to array!\n");
        flo_html_destroyUint16HashSet(&resultsSet);
        return QUERY_MEMORY_ERROR;
    }

    flo_html_destroyUint16HashSet(&resultsSet);
    return result;
}

flo_html_QueryStatus flo_html_getElementsByClassName(
    const flo_html_String className, const flo_html_Dom *dom,
    const flo_html_TextStore *textStore, flo_html_node_id **results,
    size_t *resultsLen) {
    ptrdiff_t cssQueryLen = className.len + 2;
    unsigned char cssQueryBuffer[cssQueryLen];
    flo_html_String cssQuery;
    cssQuery.buf = cssQueryBuffer;

    cssQuery.buf[0] = '.';
    memcpy(cssQuery.buf, className.buf, className.len);
    cssQuery.buf[cssQueryLen] = '\0';

    return flo_html_querySelectorAll(cssQuery, dom, textStore, results,
                                     resultsLen);
}

flo_html_QueryStatus
flo_html_getElementsByTagName(const flo_html_String tag,
                              const flo_html_Dom *dom,
                              const flo_html_TextStore *textStore,
                              flo_html_node_id **results, size_t *resultsLen) {
    return flo_html_querySelectorAll(tag, dom, textStore, results, resultsLen);
}

flo_html_QueryStatus flo_html_querySelector(const flo_html_String cssQuery,
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
flo_html_getElementByID(const flo_html_String id, const flo_html_Dom *dom,
                        const flo_html_TextStore *textStore,
                        flo_html_node_id *result) {
    ptrdiff_t cssQueryLen = id.len + 2;
    unsigned char cssQueryBuffer[cssQueryLen];
    flo_html_String cssQuery;
    cssQuery.buf = cssQueryBuffer;

    cssQuery.buf[0] = '.';
    memcpy(cssQuery.buf, id.buf, id.len);
    cssQuery.buf[cssQueryLen] = '\0';

    return flo_html_querySelector(cssQuery, dom, textStore, result);
}
