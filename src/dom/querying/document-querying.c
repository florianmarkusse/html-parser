#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "dom/querying/document-querying-util.h"
#include "dom/querying/document-querying.h"
#include "type/element/element-status.h"
#include "type/element/elements.h"
#include "utils/memory/memory.h"
#include "utils/text/text.h"

typedef enum { FREE, TAG, NUM_STATES } State;

typedef enum {
    NO_COMBINATOR,
    ADJACENT,
    CHILD,
    DESCENDANT, // Default combinator
    NUM_COMBINATORS,
} Combinator;

Combinator setCombinator(char c) {
    switch (c) {
    case '+':
        return ADJACENT;
    case '>':
        return CHILD;
    default:
        return NO_COMBINATOR;
    }
}

static inline const char *stateToString(State state) {
    static const char *stateStrings[NUM_STATES] = {
        "FREE",
        "TAG",
    };

    if (state >= 0 && state < NUM_STATES) {
        return stateStrings[state];
    }

    return "UNKNOWN";
}

QueryingStatus querySelectorAll(Document *doc, const char *cssQuery) {
    State state = FREE;
    Combinator combinator = NO_COMBINATOR;

    QueryingStatus result = QUERYING_SUCCESS;

    node_id *results = NULL;
    size_t resultsLen = 0;
    size_t currentCap = INITIAL_QUERY_CAP;

    size_t tagStart = 0;
    size_t currentPosition = 0;
    char ch = cssQuery[currentPosition];

    // Assuming that all css queries have a space between their operations
    // e.g. "body > div" and not "body>div"
    while (1) {
        // printf("Current state: %s\n", stateToString(state));
        // printf("Current char: %c\n", ch);
        // printf("char pos: %zu\n", currentPosition);

        switch (state) {
        case FREE: {
            if (isAlphaBetical(ch) || ch == '!') {
                state = TAG;
                tagStart = currentPosition;
            } else if (resultsLen > 0) {
                while (!isAlphaBetical(ch) && ch != '!') {
                    Combinator foundCombinator = setCombinator(ch);
                    if (setCombinator(ch) != NO_COMBINATOR) {
                        combinator = foundCombinator;
                    }

                    ch = cssQuery[++currentPosition];
                }

                if (combinator == NO_COMBINATOR) {
                    combinator = DESCENDANT;
                }
                currentPosition--;
            }
            break;
        }
        case TAG: {
            if (ch == ' ' || ch == '\0') {
                const size_t tagLength = currentPosition - tagStart +
                                         1; // add one for the holy spirit
                char buffer[tagLength];
                strncpy(buffer, &cssQuery[tagStart], tagLength);
                buffer[tagLength - 1] = '\0';

                element_id tagID = 0;
                if ((result = getTagID(buffer, &tagID)) != QUERYING_SUCCESS) {
                    return result;
                }

                switch (combinator) {
                case NO_COMBINATOR: {
                    if ((result = getNodesWithTagID(
                             tagID, doc, &results, &resultsLen, &currentCap)) !=
                        QUERYING_SUCCESS) {
                        return result;
                    }
                    break;
                }
                case DESCENDANT: {
                    if ((result = getDescendantsOf(&results, &resultsLen,
                                                   &currentCap, doc)) !=
                        QUERYING_SUCCESS) {
                        return result;
                    }

                    if ((result =
                             filterByTagID(tagID, doc, results, &resultsLen)) !=
                        QUERYING_SUCCESS) {
                        return result;
                    }
                }
                default:
                    break;
                }

                printf("Size of results: %zu\n", resultsLen);
                for (size_t i = 0; i < resultsLen; i++) {
                    printf("Node ID: %u, Tag ID: %u\n", results[i],
                           doc->nodes[results[i] - 1].tagID);
                }

                currentPosition--;

                combinator = NO_COMBINATOR;
                state = FREE;
            }

            break;
        }
        default: {
            break;
        }
        }

        if (ch == '\0') {
            break;
        }

        ch = cssQuery[++currentPosition];
    }

    return result;
}
