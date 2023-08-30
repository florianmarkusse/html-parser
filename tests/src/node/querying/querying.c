#include <stdio.h>

#include "node/querying/querying.h"
#include "test.h"

bool testNodeQueries(size_t *successes, size_t *failures) {
    printTestTopicStart("node queries");

    size_t localSuccesses = 0;
    size_t localFailures = 0;

    testBoolNodeQueries(&localSuccesses, &localFailures);
    testCharNodeQueries(&localSuccesses, &localFailures);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
