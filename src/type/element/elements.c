#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/node/node.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/text/string.h"

flo_html_StringRegistry
flo_html_initStringRegistry(const ptrdiff_t stringsCapacity,
                            flo_html_Arena *perm) {
    return (flo_html_StringRegistry){
        .set = flo_html_initStringHashSet(stringsCapacity, perm)};
}

#define FLO_HTML_MAX_ELEMENTS_PER_REGISTRY 1U << 13U
// TODO(florian): USE MORE SENSIBLE VALUES THAN FLO_HTML_TOTAL_ELEMENTS
flo_html_TextStore flo_html_createTextStore(flo_html_Arena *perm) {
    return (flo_html_TextStore){
        .tags = flo_html_initStringRegistry(FLO_HTML_MAX_ELEMENTS_PER_REGISTRY,
                                            perm),
        .boolProps = flo_html_initStringRegistry(
            FLO_HTML_MAX_ELEMENTS_PER_REGISTRY, perm),
        .propKeys = flo_html_initStringRegistry(
            FLO_HTML_MAX_ELEMENTS_PER_REGISTRY, perm),
        .propValues = flo_html_initStringRegistry(
            FLO_HTML_MAX_ELEMENTS_PER_REGISTRY, perm),
    };
}
