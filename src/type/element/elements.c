#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/node/node.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/text/string.h"

flo_html_TextStore flo_html_createTextStore(flo_html_Arena *perm) {
    return (flo_html_TextStore){
        .tags = flo_html_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm),
        .boolProps =
            flo_html_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm),
        .propKeys =
            flo_html_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm),
        .propValues =
            flo_html_initStringHashSet(FLO_HTML_REGISTRY_START_SIZE, perm),
    };
}
