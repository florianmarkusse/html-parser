#include <stdio.h>

#include "node/querying/querying.h"
#include "test.h"

bool testNodeQueries(ptrdiff_t *successes, ptrdiff_t *failures,
                     flo_Arena scratch) {
    printTestTopicStart(FLO_STRING("node queries"));

    ptrdiff_t localSuccesses = 0;
    ptrdiff_t localFailures = 0;

    testBoolNodeQueries(&localSuccesses, &localFailures, scratch);
    testCharNodeQueries(&localSuccesses, &localFailures, scratch);
    testArrayNodeQueries(&localSuccesses, &localFailures, scratch);

    printTestScore(localSuccesses, localFailures);

    *successes += localSuccesses;
    *failures += localFailures;

    return localFailures > 0;
}
