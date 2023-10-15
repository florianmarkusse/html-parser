#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/node/node.h"
#include "flo/html-parser/type/data/data-page.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/text/string.h"

flo_html_StringRegistry
flo_html_initStringRegistry(const ptrdiff_t stringsCapacity,
                            const ptrdiff_t pageSize, flo_html_Arena *perm) {
    return (flo_html_StringRegistry){
        .set = flo_html_initStringHashSet(stringsCapacity, perm),
        .dataPage = flo_html_initDataPage(pageSize, perm)};
}

// TODO(florian): USE MORE SENSIBLE VALUES THAN FLO_HTML_TOTAL_ELEMENTS
flo_html_TextStore flo_html_createTextStore(flo_html_Arena *perm) {
    return (flo_html_TextStore){
        .tags = flo_html_initStringRegistry(FLO_HTML_MAX_ELEMENTS_PER_REGISTRY,
                                            FLO_HTML_TAGS_PAGE_SIZE, perm),
        .boolProps =
            flo_html_initStringRegistry(FLO_HTML_MAX_ELEMENTS_PER_REGISTRY,
                                        FLO_HTML_BOOL_PROPS_PAGE_SIZE, perm),
        .propKeys =
            flo_html_initStringRegistry(FLO_HTML_MAX_ELEMENTS_PER_REGISTRY,
                                        FLO_HTML_PROP_KEYS_PAGE_SIZE, perm),
        .propValues =
            flo_html_initStringRegistry(FLO_HTML_MAX_ELEMENTS_PER_REGISTRY,
                                        FLO_HTML_PROP_VALUES_PAGE_SIZE, perm),
        .text = flo_html_initDataPage(FLO_HTML_TEXT_PAGE_SIZE, perm),
    };
}
