#include <stddef.h>
#include <stdio.h>
#include <string.h>

#include "dom/querying/document-querying.h"
#include "type/element/element-status.h"
#include "type/element/elements.h"
#include "utils/text/text.h"

typedef enum { SPACE, TAG, NUM_STATES } State;

typedef enum {
    NONE,
    QUERY_TAG,
    NUM_PARTS,
} QueryPart;

static inline const char *stateToString(State state) {
    static const char *stateStrings[NUM_STATES] = {
        "SPACE",
        "TAG",
    };

    if (state >= 0 && state < NUM_STATES) {
        return stateStrings[state];
    }

    return "UNKNOWN";
}

// TODO(florian): something with offset or just rip through both probs.
static inline ElementStatus getTagID(const char *tag, element_id *tagID) {
    return findElement(&gText.container, &gText.len, tag, 0, tagID);
}

QueryingStatus querySelectorAll(Document *doc, const char *cssQuery) {
    State state = SPACE;
    QueryPart previousPart = NONE;

    size_t tagStart = 0;
    size_t currentPosition = 0;
    char ch = cssQuery[currentPosition];
    while (1) {
        printf("Current state: %s\n", stateToString(state));

        switch (state) {
        case SPACE: {
            if (isAlphaBetical(ch) || ch == '!') {
                state = TAG;
                tagStart = currentPosition;
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

                printf("found tag: %s\n", buffer);
                element_id tagID = 0;
                printf("With tag iD: %u\n", tagID);
                if (getTagID(buffer, &tagID) == ELEMENT_SUCCESS) {
                    printf("With tag iD: %u\n", tagID);
                }

                state = SPACE;
                previousPart = QUERY_TAG;
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

    return QUERYING_SUCCESS;
}
