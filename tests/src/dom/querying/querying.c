#include <stdio.h>

#include "dom/querying/querying.h"
#include "test.h"

unsigned char testDomQueries(size_t *successes, size_t *failures) {
    printTestTopicStart("DOM queries");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    testQuerySelectorAll(&localSuccesses, &localFailures);
    testQuerySelector(&localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}