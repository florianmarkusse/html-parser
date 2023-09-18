#ifndef FLO_HTML_PARSER_H
#define FLO_HTML_PARSER_H

#ifdef __cplusplus
extern "C" {
#endif

// Node reading functions
#include "flo/html-parser/dom/reading/reading.h"

// Node modification functions
#include "flo/html-parser/dom/appendix/appendix.h"
#include "flo/html-parser/dom/deletion/deletion.h"
#include "flo/html-parser/dom/modification/modification.h"
#include "flo/html-parser/dom/prependix/prependix.h"
#include "flo/html-parser/dom/replacement/replacement.h"

// HTML document comparison
#include "flo/html-parser/dom/comparison/comparison.h"

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
