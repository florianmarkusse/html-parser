#include <stdio.h>

#include "dom/querying/querying.h"
#include "test.h"

void testflo_html_DomQueries(flo_Arena scratch) {
    FLO_TEST_TOPIC(FLO_STRING("DOM queries")) {
        testQuerySelectorAll(scratch);
        testQuerySelector(scratch);
    }
}
