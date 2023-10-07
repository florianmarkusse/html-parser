#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/utils/memory/memory.h"
#include "flo/html-parser/utils/text/string.h"
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

bool isTagStartChar(const char ch) {
    return flo_html_isAlphaBetical(ch) || ch == '!';
}

bool isElementStartChar(const char ch) {
    return isTagStartChar(ch) || ch == '.' || ch == '#';
}

bool isSpecifiedCombinator(const char ch) {
    return ch == '>' || ch == '+' || ch == '~';
}

bool isCombinator(const char ch) {
    return ch == ' ' || isSpecifiedCombinator(ch);
}

bool endOfCurrentFilter(const char ch) {
    return isCombinator(ch) || flo_html_isSpecialSpace(ch) || ch == '[' ||
           ch == '.' || ch == '#';
}

static ptrdiff_t parseEmptyContent(flo_html_String css, ptrdiff_t start) {
    ptrdiff_t end = start;
    unsigned char ch = flo_html_getChar(css, end);

    while (end < css.len && !isElementStartChar(ch) && ch != '[' && ch != '*') {
        ch = flo_html_getChar(css, ++end);
    }

    return end;
}

ptrdiff_t parseToken(const flo_html_String css, ptrdiff_t currentPosition,
                     flo_html_String *token) {
    ptrdiff_t tokenStart = currentPosition;
    unsigned char ch = flo_html_getChar(css, currentPosition);
    while (currentPosition < css.len && ch != ' ' &&
           !flo_html_isSpecialSpace(ch) && ch != '=' && ch != ']') {
        ch = flo_html_getChar(css, ++currentPosition);
    }
    ptrdiff_t tokenLength = currentPosition - tokenStart;

    (*token).buf = flo_html_getCharPtr(css, tokenStart);
    (*token).len = tokenLength;

    while (currentPosition < css.len && ch != '=' && ch != ']' && ch != '.' &&
           ch != '#') {
        ch = flo_html_getChar(css, ++currentPosition);
    }

    return currentPosition;
}

flo_html_QueryStatus getQueryResults(const flo_html_String css,
                                     const flo_html_Dom *dom,
                                     const flo_html_TextStore *textStore,
                                     flo_html_Uint16HashSet *set) {
    flo_html_QueryStatus result = QUERY_SUCCESS;

    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT];
    ptrdiff_t filtersLen = 0;

    flo_html_Combinator currentflo_html_Combinator = NO_COMBINATOR;
    Selector currentSelector = NORMAL;

    ptrdiff_t currentPosition = 0;
    currentPosition = parseEmptyContent(css, currentPosition);

    unsigned char ch;
    while (currentPosition < css.len) {
        ch = flo_html_getChar(css, currentPosition);

        if (isTagStartChar(ch)) {
            CHECK_FILTERS_LIMIT(filtersLen);

            ptrdiff_t tagStart = currentPosition;
            while (currentPosition < css.len && !endOfCurrentFilter(ch)) {
                ch = flo_html_getChar(css, ++currentPosition);
            }
            ptrdiff_t tagLen = currentPosition - tagStart;

            flo_html_element_id tagID = flo_html_getTagID(
                FLO_HTML_S_LEN(flo_html_getCharPtr(css, tagStart), tagLen),
                textStore);
            if (tagID == 0) {
                return QUERY_NOT_SEEN_BEFORE;
            }

            filters[filtersLen].attributeSelector = TAG;
            filters[filtersLen].data.tagID = tagID;
            filtersLen++;
        } else if (ch == '*') {
            CHECK_FILTERS_LIMIT(filtersLen);

            while (currentPosition < css.len && !endOfCurrentFilter(ch)) {
                ch = flo_html_getChar(css, ++currentPosition);
            }

            filters[filtersLen].attributeSelector = ALL_NODES;
            filtersLen++;
        }

        while (ch == '[' || ch == '.' || ch == '#') {
            CHECK_FILTERS_LIMIT(filtersLen);

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
            }

            while (currentPosition < css.len && !isTagStartChar(ch)) {
                ch = flo_html_getChar(css, ++currentPosition);
            }

            flo_html_String token;
            currentPosition = parseToken(css, currentPosition, &token);
            ch = flo_html_getChar(css, currentPosition);

            // If ch == '.' or ch == '#', it means we had to have had '.' or '#'
            // at the beginnging. This is a bit of a funky inference to make,
            // but it is correct (I think).
            if (currentSelector == CLASS || currentSelector == ID) {
                const flo_html_String keyBuffer = currentSelector == CLASS
                                                      ? FLO_HTML_S("class")
                                                      : FLO_HTML_S("id");
                flo_html_element_id propKeyID =
                    flo_html_getPropKeyID(keyBuffer, textStore);
                if (propKeyID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                flo_html_element_id propValueID =
                    flo_html_getPropValueID(token, textStore);
                if (propValueID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                filters[filtersLen].attributeSelector = PROPERTY;
                filters[filtersLen].data.keyValuePair.keyID = propKeyID;
                filters[filtersLen].data.keyValuePair.valueID = propValueID;
                filtersLen++;
            } else if (ch == ']') {
                flo_html_element_id boolPropID =
                    flo_html_getBoolPropID(token, textStore);
                if (boolPropID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                filters[filtersLen].attributeSelector = BOOLEAN_PROPERTY;
                filters[filtersLen].data.propID = boolPropID;
                filtersLen++;
            } else if (ch == '=') {
                flo_html_element_id propKeyID =
                    flo_html_getPropKeyID(token, textStore);
                if (propKeyID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                // Skip the '='
                ch = flo_html_getChar(css, ++currentPosition);
                while (currentPosition < css.len && !isTagStartChar(ch)) {
                    ch = flo_html_getChar(css, ++currentPosition);
                }

                flo_html_String propValue;
                currentPosition = parseToken(css, currentPosition, &propValue);
                ch = flo_html_getChar(css, currentPosition);

                flo_html_element_id propValueID =
                    flo_html_getPropValueID(propValue, textStore);
                if (propValueID == 0) {
                    return QUERY_NOT_SEEN_BEFORE;
                }

                filters[filtersLen].attributeSelector = PROPERTY;
                filters[filtersLen].data.keyValuePair.keyID = propKeyID;
                filters[filtersLen].data.keyValuePair.valueID = propValueID;
                filtersLen++;
            } else {
                FLO_HTML_PRINT_ERROR("Unrecognized character in filtering "
                                     "function. Dropping a token!\n");
            }

            if (ch == ']') {
                ch = flo_html_getChar(css, ++currentPosition);
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
            if ((result = flo_html_getFilteredAdjacents(
                     filters, filtersLen, dom, PTRDIFF_MAX, set)) !=
                QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case DESCENDANT: {
            if ((result = flo_html_getFilteredDescendants(
                     filters, filtersLen, dom, PTRDIFF_MAX, set)) !=
                QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        }

        filtersLen = 0;

        // Swoop up any possible next combinator and move on to a potentially
        // next element.
        char combinator = ' ';
        while (currentPosition < css.len && !isElementStartChar(ch) &&
               ch != '[') {
            if (isSpecifiedCombinator(ch)) {
                combinator = ch;
            }
            ch = flo_html_getChar(css, ++currentPosition);
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
        }
    }

    return result;
}

flo_html_QueryStatus
flo_html_querySelectorAll(const flo_html_String css, const flo_html_Dom *dom,
                          const flo_html_TextStore *textStore,
                          flo_html_node_id **results, ptrdiff_t *resultsLen) {
    flo_html_Uint16HashSet resultsSet;
    if (flo_html_initUint16HashSet(&resultsSet, FLO_HTML_INITIAL_QUERY_CAP) !=
        HASH_SUCCESS) {
        FLO_HTML_PRINT_ERROR(
            "Failed to allocate memory initializating querySelectorAll\n");
        return QUERY_MEMORY_ERROR;
    }

    flo_html_QueryStatus result = QUERY_SUCCESS;
    ptrdiff_t currentQueryStart = 0;
    ptrdiff_t currentQueryEnd =
        flo_html_firstOccurenceOfFrom(css, ',', currentQueryStart);

    if (currentQueryEnd > 0) {
        flo_html_Uint16HashSet set;
        if (flo_html_initUint16HashSet(&set, FLO_HTML_INITIAL_QUERY_CAP) !=
            HASH_SUCCESS) {
            FLO_HTML_PRINT_ERROR(
                "Failed to allocate memory initializating querySelectorAll\n");
            return QUERY_MEMORY_ERROR;
        }

        while (currentQueryEnd >= 0) {
            flo_html_String singularQuery =
                FLO_HTML_S_LEN(css.buf + currentQueryStart,
                               currentQueryEnd - currentQueryStart);

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

            currentQueryStart = currentQueryEnd + 1; // skip ','
            currentQueryEnd =
                flo_html_firstOccurenceOfFrom(css, ',', currentQueryStart);
        }
        flo_html_String singularQuery = FLO_HTML_S_LEN(
            css.buf + currentQueryStart, css.len - currentQueryStart);

        if ((result = getQueryResults(singularQuery, dom, textStore, &set)) !=
            QUERY_SUCCESS) {
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

        flo_html_destroyUint16HashSet(&set);
    } else {
        if ((result = getQueryResults(css, dom, textStore, &resultsSet)) !=
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
    ptrdiff_t *resultsLen) {
    ptrdiff_t cssLen = className.len + 1;
    unsigned char cssBuffer[cssLen];
    flo_html_String css;
    css.buf = cssBuffer;

    css.buf[0] = '.';
    memcpy(css.buf, className.buf, className.len);

    return flo_html_querySelectorAll(css, dom, textStore, results, resultsLen);
}

flo_html_QueryStatus flo_html_getElementsByTagName(
    const flo_html_String tag, const flo_html_Dom *dom,
    const flo_html_TextStore *textStore, flo_html_node_id **results,
    ptrdiff_t *resultsLen) {
    return flo_html_querySelectorAll(tag, dom, textStore, results, resultsLen);
}

flo_html_QueryStatus flo_html_querySelector(const flo_html_String css,
                                            const flo_html_Dom *dom,
                                            const flo_html_TextStore *textStore,
                                            flo_html_node_id *result) {
    flo_html_node_id *results = NULL;
    ptrdiff_t resultsLen = 0;

    flo_html_QueryStatus status =
        flo_html_querySelectorAll(css, dom, textStore, &results, &resultsLen);
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
        for (ptrdiff_t i = 0; i < resultsLen; i++) {
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
    ptrdiff_t cssLen = id.len + 1;
    unsigned char cssBuffer[cssLen];
    flo_html_String css;
    css.buf = cssBuffer;

    css.buf[0] = '.';
    memcpy(css.buf, id.buf, id.len);

    return flo_html_querySelector(css, dom, textStore, result);
}
