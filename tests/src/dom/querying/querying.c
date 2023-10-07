#include <stdio.h>

#include "dom/querying/querying.h"
#include "test.h"

unsigned char testflo_html_DomQueries(ptrdiff_t *successes,
                                      ptrdiff_t *failures) {
    printTestTopicStart("DOM queries");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    testQuerySelectorAll(&localSuccesses, &localFailures);
    testQuerySelector(&localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
