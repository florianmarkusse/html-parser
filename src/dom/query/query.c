#include "flo/html-parser/dom/dom.h"
#include "flo/html-parser/dom/query/query-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/writing.h"
#include "flo/html-parser/util/parse.h"
#include "memory/arena.h"
#include "text/char.h"
#include "text/string.h"
#include <stdint.h>

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
            FLO_PRINT_ERROR(                                                   \
                "Too many filters in a single element detected!\n");           \
            return QUERY_TOO_MANY_ELEMENT_FILTERS;                             \
        }                                                                      \
    } while (0)

#define GET_PROPERTY_ID_OR_RETURN(variable, key, set)                          \
    do {                                                                       \
        (variable) =                                                           \
            (flo_html_index_id)flo_trie_containsStringAutoUint16Map(key, set); \
        if ((variable) == 0) {                                                 \
            return QUERY_NOT_SEEN_BEFORE;                                      \
        }                                                                      \
    } while (0)

static inline bool isTagStartChar(unsigned char ch) {
    return flo_isAlphabetical(ch) || ch == '!';
}

static inline bool isElementStartChar(unsigned char ch) {
    return isTagStartChar(ch) || ch == '.' || ch == '#';
}

static inline bool isSpecifiedCombinator(unsigned char ch) {
    return ch == '>' || ch == '+' || ch == '~';
}

static inline bool isCombinator(unsigned char ch) {
    return ch == ' ' || isSpecifiedCombinator(ch);
}

static inline bool endOfCurrentFilter(unsigned char ch) {
    return isCombinator(ch) || flo_isFormattingCharacter(ch) || ch == '[' ||
           ch == '.' || ch == '#';
}

#define PARSE_EMPTY_CONTENT(ps)                                                \
    FLO_PARSE_NEXT_CHAR_UNTIL(ps, isElementStartChar(ch) || ch == '[' ||       \
                                      ch == '*')

flo_String parseToken(flo_parse_Status *ps) {
    ptrdiff_t tokenStart = ps->idx;
    FLO_PARSE_NEXT_CHAR_UNTIL(*ps, ch == ' ' || flo_isFormattingCharacter(ch) ||
                                       ch == '=' || ch == ']');
    ptrdiff_t tokenLength = ps->idx - tokenStart;

    flo_String token = (flo_String){.buf = flo_getCharPtr(ps->text, tokenStart),
                                    .len = tokenLength};

    FLO_PARSE_NEXT_CHAR_UNTIL(*ps,
                              ch == '=' || ch == ']' || ch == '.' || ch == '#');

    return token;
}

flo_html_QueryStatus getQueryResults(flo_String css, flo_html_Dom *dom,
                                     flo_Uint16HashSet *set, flo_Arena *perm) {
    flo_parse_Status ps = (flo_parse_Status){.idx = 0, .text = css};

    flo_html_QueryStatus result = QUERY_SUCCESS;

    flo_html_FilterType filters[FLO_HTML_MAX_FILTERS_PER_ELEMENT];
    ptrdiff_t filtersLen = 0;

    flo_html_Combinator currentflo_html_Combinator = NO_COMBINATOR;
    Selector currentSelector = NORMAL;

    PARSE_EMPTY_CONTENT(ps);

    while (ps.idx < ps.text.len) {
        unsigned char ch = flo_parse_currentChar(ps);
        if (isTagStartChar(ch)) {
            CHECK_FILTERS_LIMIT(filtersLen);

            ptrdiff_t tagStart = ps.idx;
            FLO_PARSE_NEXT_CHAR_UNTIL(ps, endOfCurrentFilter(ch));
            ptrdiff_t tagLen = ps.idx - tagStart;

            flo_html_index_id tagID;
            GET_PROPERTY_ID_OR_RETURN(
                tagID, FLO_STRING_LEN(flo_getCharPtr(css, tagStart), tagLen),
                &dom->tagMap);

            filters[filtersLen].attributeSelector = TAG;
            filters[filtersLen].data.tagID = tagID;
            filtersLen++;
        } else if (ch == '*') {
            CHECK_FILTERS_LIMIT(filtersLen);
            FLO_PARSE_NEXT_CHAR_UNTIL(ps, endOfCurrentFilter(ch));

            filters[filtersLen].attributeSelector = ALL_NODES;
            filtersLen++;
        }

        FLO_PARSE_PARSE_CHAR_WHILE(ps, ch == '[' || ch == '.' || ch == '#', {
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

            FLO_PARSE_NEXT_CHAR_UNTIL(ps, isTagStartChar(ch));
            flo_String token = parseToken(&ps);
            ch = flo_parse_currentChar(ps);

            // If ch == '.' or ch == '#', it means we had to have had '.' or '#'
            // at the beginnging. This is a bit of a funky inference to make,
            // but it is correct (I think).
            if (currentSelector == CLASS || currentSelector == ID) {
                flo_String keyBuffer = currentSelector == CLASS
                                           ? FLO_STRING("class")
                                           : FLO_STRING("id");
                flo_html_index_id propKeyID;
                GET_PROPERTY_ID_OR_RETURN(propKeyID, keyBuffer,
                                          &dom->propKeyMap);

                flo_html_index_id propValueID;
                GET_PROPERTY_ID_OR_RETURN(propValueID, token,
                                          &dom->propValueMap);

                filters[filtersLen].attributeSelector = PROPERTY;
                filters[filtersLen].data.keyValuePair.keyID = propKeyID;
                filters[filtersLen].data.keyValuePair.valueID = propValueID;
                filtersLen++;
            } else if (ch == ']') {
                flo_html_index_id boolPropID;
                GET_PROPERTY_ID_OR_RETURN(boolPropID, token, &dom->boolPropMap);

                filters[filtersLen].attributeSelector = BOOLEAN_PROPERTY;
                filters[filtersLen].data.propID = boolPropID;
                filtersLen++;
            } else if (ch == '=') {
                flo_html_index_id propKeyID;
                GET_PROPERTY_ID_OR_RETURN(propKeyID, token, &dom->propKeyMap);

                // Skip the '='
                ps.idx++;
                FLO_PARSE_NEXT_CHAR_UNTIL(ps, isTagStartChar(ch));

                flo_String propValue = parseToken(&ps);
                ch = flo_parse_currentChar(ps);

                flo_html_index_id propValueID;
                GET_PROPERTY_ID_OR_RETURN(propValueID, propValue,
                                          &dom->propValueMap);

                filters[filtersLen].attributeSelector = PROPERTY;
                filters[filtersLen].data.keyValuePair.keyID = propKeyID;
                filters[filtersLen].data.keyValuePair.valueID = propValueID;
                filtersLen++;
            } else {
                FLO_PRINT_ERROR("Unrecognized character in filtering "
                                "function. Dropping a token!\n");
            }

            if (ch == ']') {
                ps.idx++;
            }
        });

        if (filtersLen < 1) {
            FLO_PRINT_ERROR("Did not receive any filters in the css query\n");
            return QUERY_INVALID_ELEMENT;
        }

        // Do filtering :)
        switch (currentflo_html_Combinator) {
        case NO_COMBINATOR: {
            if (!flo_html_getNodesWithoutflo_html_Combinator(
                    filters, filtersLen, dom, set, perm)) {
                return QUERY_MEMORY_ERROR;
            }
            break;
        }
        case ADJACENT: {
            if ((result = flo_html_getFilteredAdjacents(
                     filters, filtersLen, dom, 1,

                     set, perm)) != QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case CHILD: {
            if ((result = flo_html_getFilteredDescendants(filters, filtersLen,
                                                          dom, 1, set, perm)) !=
                QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case GENERAL_SIBLING: {
            if ((result = flo_html_getFilteredAdjacents(
                     filters, filtersLen, dom, PTRDIFF_MAX, set, perm)) !=
                QUERY_SUCCESS) {
                return result;
            }
            break;
        }
        case DESCENDANT: {
            if ((result = flo_html_getFilteredDescendants(
                     filters, filtersLen, dom, PTRDIFF_MAX, set, perm)) !=
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
        FLO_PARSE_NEXT_CHAR_UNTIL(ps, isElementStartChar(ch) || ch == '[', {
            if (isSpecifiedCombinator(ch)) {
                combinator = ch;
            }
        });

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

flo_html_QueryStatus flo_html_querySelectorAll(flo_String css,
                                               flo_html_Dom *dom,
                                               flo_html_node_id_a *results,
                                               flo_Arena *perm) {
    {
        flo_Arena scratch = *perm;
        flo_Uint16HashSet resultsSet =
            flo_initUint16HashSet(FLO_HTML_INITIAL_QUERY_CAP, &scratch);

        flo_html_QueryStatus result = QUERY_SUCCESS;
        flo_Uint16HashSet set =
            flo_initUint16HashSet(FLO_HTML_INITIAL_QUERY_CAP, &scratch);

        ptrdiff_t from = 0;
        while (from < css.len) {
            flo_String iter = flo_splitString(css, ',', from);

            if ((result = getQueryResults(iter, dom, &set, &scratch)) !=
                QUERY_SUCCESS) {
                FLO_ERROR_WITH_CODE_ONLY(
                    flo_html_queryingStatusToString(result),
                    "Unable get query results!\n");
                return result;
            }

            flo_Uint16HashSetIterator iterator =
                (flo_Uint16HashSetIterator){.set = &set, .index = 0};
            flo_html_node_id nodeIDResult;
            while ((nodeIDResult = flo_nextUint16HashSetIterator(&iterator)) !=
                   0) {
                if (!flo_insertUint16HashSet(&resultsSet, nodeIDResult,
                                             &scratch)) {
                    FLO_PRINT_ERROR("Failed to save intermediate results!\n");

                    return QUERY_MEMORY_ERROR;
                }
            }

            flo_resetUint16HashSet(&set);

            from += iter.len + 1;
        }

        // create on scratch arena by conversion.
        flo_uint16_t_a array = flo_uint16HashSetToArray(&resultsSet, &scratch);

        // copy to perm arena
        results->buf = (flo_html_node_id *)flo_copyToArena(
            perm, array.buf, FLO_SIZEOF(flo_html_node_id),
            FLO_ALIGNOF(flo_html_node_id), array.len);
        results->len = array.len;
    }

    return QUERY_SUCCESS;
}

flo_html_QueryStatus flo_html_querySelector(flo_String css, flo_html_Dom *dom,
                                            flo_html_node_id *result,
                                            flo_Arena scratch) {
    flo_html_node_id_a *results = FLO_NEW(&scratch, flo_html_node_id_a);

    flo_html_QueryStatus status =
        flo_html_querySelectorAll(css, dom, results, &scratch);
    if (status != QUERY_SUCCESS) {
        return status;
    }

    if (results->len == 0) {
        *result = 0;
        return QUERY_SUCCESS;
    }

    flo_html_node_id currentNode = FLO_HTML_ROOT_NODE_ID;
    while (currentNode) {
        for (ptrdiff_t i = 0; i < results->len; i++) {
            if (results->buf[i] == currentNode) {
                *result = currentNode;
                return QUERY_SUCCESS;
            }
        }
        currentNode = flo_html_traverseDom(currentNode, dom);
    }

    return status;
}
