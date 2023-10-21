
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/util/assert.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

// TODO: remove this
void flo_html_addRegistration(flo_html_HashElement hashElement,
                              flo_html_HashElement_d_a *hashElements,
                              flo_html_Arena *perm) {
    *FLO_HTML_PUSH(hashElements, perm) = hashElement;
}

// TODO: remove this
void flo_html_addTagRegistration(bool isPaired,
                                 flo_html_HashElement hashElement,
                                 flo_html_Dom *dom, flo_html_Arena *perm) {
    *FLO_HTML_PUSH(&dom->tagRegistry, perm) = (flo_html_TagRegistration){
        .isPaired = isPaired, .hashElement = hashElement};
}
