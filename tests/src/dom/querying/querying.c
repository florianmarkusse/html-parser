#include <stdio.h>

#include "dom/querying/querying.h"
#include "test.h"

unsigned char testflo_html_DomQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                                      flo_Arena scratch) {
    printTestTopicStart("DOM queries");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    testQuerySelectorAll(&localSuccesses, &localFailures, scratch);
    testQuerySelector(&localSuccesses, &localFailures, scratch);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
