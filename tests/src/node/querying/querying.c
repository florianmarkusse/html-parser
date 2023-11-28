#include <stdio.h>

#include "node/querying/querying.h"
#include "test.h"

void testNodeQueries(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("node queries")) {
        testBoolNodeQueries(scratch);
        testCharNodeQueries(scratch);
        testArrayNodeQueries(scratch);
    }
}
