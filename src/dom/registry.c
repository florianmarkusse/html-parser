
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

void flo_html_addTagRegistration(const flo_html_index_id tagID,
                                 const bool isPaired,
                                 const flo_html_HashElement *hashElement,
                                 flo_html_Dom *dom) {
    // TODO: dynamic
    if (dom->tagRegistryLen >= dom->tagRegistryCap) {
        FLO_HTML_PRINT_ERROR("THE TAG REGISTRY IS OVERFLOWING\n");
    }

    flo_html_TagRegistration *tagRegistry =
        &(dom->tagRegistry[dom->tagRegistryLen]);
    tagRegistry->tagID = tagID;
    tagRegistry->isPaired = isPaired;
    tagRegistry->hashElement.hash = hashElement->hash;
    tagRegistry->hashElement.offset = hashElement->offset;

    dom->tagRegistryLen++;
}

void flo_html_addRegistration(const flo_html_HashElement *hashElement,
                              flo_html_BasicRegistry *basicRegistry) {
    // TODO: dynamic
    if (basicRegistry->len >= basicRegistry->cap) {
        FLO_HTML_PRINT_ERROR("THE REGISTRY IS OVERFLOWING\n");
    }

    flo_html_HashElement *newHashElement =
        &(basicRegistry->hashes[basicRegistry->len]);
    newHashElement->hash = hashElement->hash;
    newHashElement->offset = hashElement->offset;

    basicRegistry->len++;
}
