#include <flo/html-parser/dom/dom-user.h>
#include <flo/html-parser/dom/dom-writing.h>
#include <flo/html-parser/dom/dom.h>
#include <flo/html-parser/dom/query/dom-query.h>
#include <flo/html-parser/type/element/elements.h>
#include <flo/html-parser/utils/print/error.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "test-status.h"
#include "test.h"

#define CURRENT_DIR "tests/src/node/querying/inputs/"
#define TEST_FILE_1 CURRENT_DIR "test-1.html"
