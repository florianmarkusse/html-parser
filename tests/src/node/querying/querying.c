#include <stdio.h>

#include "node/querying/querying.h"
#include "test.h"

bool testNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures) {
    printTestTopicStart("node queries");

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    testBoolNodeQueries(&localSuccesses, &localFailures);
    testCharNodeQueries(&localSuccesses, &localFailures);
    testArrayNodeQueries(&localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
