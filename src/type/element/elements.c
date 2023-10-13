#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/node/node.h"
#include "flo/html-parser/type/data/data-page.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/text/string.h"

void flo_html_initStringRegistry(flo_html_StringRegistry *stringRegistry,
                                 const ptrdiff_t stringsCapacity,
                                 const ptrdiff_t pageSize,
                                 flo_html_Arena *perm) {
    flo_html_initStringHashSet(&stringRegistry->set, stringsCapacity, perm);
    flo_html_initDataPage(&stringRegistry->dataPage, pageSize, perm);
}

void flo_html_destroyStringRegistry(flo_html_StringRegistry *stringRegistry) {
    flo_html_destroyStringHashSet(&stringRegistry->set);
    flo_html_destroyDataPage(&stringRegistry->dataPage);
}

// TODO(florian): USE MORE SENSIBLE VALUES THAN FLO_HTML_TOTAL_ELEMENTS
void flo_html_createTextStore(flo_html_TextStore *textStore,
                              flo_html_Arena *perm) {
    flo_html_initStringRegistry(&textStore->tags,
                                FLO_HTML_MAX_ELEMENTS_PER_REGISTRY,
                                FLO_HTML_TAGS_PAGE_SIZE, perm);
    flo_html_initStringRegistry(&textStore->boolProps,
                                FLO_HTML_MAX_ELEMENTS_PER_REGISTRY,
                                FLO_HTML_BOOL_PROPS_PAGE_SIZE, perm);
    flo_html_initStringRegistry(&textStore->propKeys,
                                FLO_HTML_MAX_ELEMENTS_PER_REGISTRY,
                                FLO_HTML_PROP_KEYS_PAGE_SIZE, perm);
    flo_html_initStringRegistry(&textStore->propValues,
                                FLO_HTML_MAX_ELEMENTS_PER_REGISTRY,
                                FLO_HTML_PROP_VALUES_PAGE_SIZE, perm);

    flo_html_initDataPage(&textStore->text, FLO_HTML_TEXT_PAGE_SIZE, perm);
}

flo_html_ElementIndex
flo_html_elementToIndex(flo_html_StringRegistry *stringRegistry,
                        const flo_html_String element) {
    flo_html_ElementIndex result =
        flo_html_containsStringHashSet(&stringRegistry->set, element);
    if (result.entryIndex > 0) {
        return result;
    }

    result.entryIndex = flo_html_insertIntoPageWithHash(
        element, &stringRegistry->dataPage, &stringRegistry->set,
        &result.hashElement);

    return result;
}
