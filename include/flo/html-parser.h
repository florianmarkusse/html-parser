#ifndef FLO_HTML_PARSER_H
#define FLO_HTML_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

// Node reading functions
#include "flo/html-parser/dom/reading/reading.h"

// Node modification functions
#include "flo/html-parser/dom/appendix.h"
#include "flo/html-parser/dom/deletion.h"
#include "flo/html-parser/dom/modification.h"
#include "flo/html-parser/dom/prependix.h"
#include "flo/html-parser/dom/replacement.h"

// HTML document comparison
#include "flo/html-parser/dom/comparison.h"

// HTML document traversing
#include "flo/html-parser/dom/traversal.h"

// HTML document querying
#include "flo/html-parser/dom/query/query.h"

// HTML document parsing
#include "flo/html-parser/dom/dom.h"

// HTML document writing
#include "flo/html-parser/dom/writing.h"

#ifdef __cplusplus
}
#endif

#endif
