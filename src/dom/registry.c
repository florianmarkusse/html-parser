
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/util/memory.h"

flo_html_DomStatus
flo_html_addTagRegistration(const flo_html_indexID tagID, const bool isPaired,
                            const flo_html_HashElement *hashElement,
                            flo_html_Dom *dom) {
    if ((dom->tagRegistry = flo_html_resizeArray(
             dom->tagRegistry, dom->tagRegistryLen, &dom->tagRegistryCap,
             sizeof(flo_html_TagRegistration),
             FLO_HTML_TAG_REGISTRATIONS_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    flo_html_TagRegistration *tagRegistry =
        &(dom->tagRegistry[dom->tagRegistryLen]);
    tagRegistry->tagID = tagID;
    tagRegistry->isPaired = isPaired;
    tagRegistry->hashElement.hash = hashElement->hash;
    tagRegistry->hashElement.offset = hashElement->offset;

    dom->tagRegistryLen++;
    return DOM_SUCCESS;
}

flo_html_DomStatus
flo_html_addRegistration(const flo_html_HashElement *hashElement,
                         flo_html_BasicRegistry *basicRegistry) {
    if ((basicRegistry->hashes = flo_html_resizeArray(
             basicRegistry->hashes, basicRegistry->len, &basicRegistry->cap,
             sizeof(flo_html_HashElement),
             FLO_HTML_PROP_REGISTRATIONS_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    flo_html_HashElement *newHashElement =
        &(basicRegistry->hashes[basicRegistry->len]);
    newHashElement->hash = hashElement->hash;
    newHashElement->offset = hashElement->offset;

    basicRegistry->len++;
    return DOM_SUCCESS;
}

flo_html_DomStatus
flo_html_addBoolPropRegistration(const flo_html_HashElement *hashElement,
                                 flo_html_Dom *dom) {
    return flo_html_addRegistration(hashElement, &dom->boolPropRegistry);
}

flo_html_DomStatus
flo_html_addPropKeyRegistration(const flo_html_HashElement *hashElement,
                                flo_html_Dom *dom) {
    return flo_html_addRegistration(hashElement, &dom->propKeyRegistry);
}

flo_html_DomStatus
flo_html_addPropValueRegistration(const flo_html_HashElement *hashElement,
                                  flo_html_Dom *dom) {
    return flo_html_addRegistration(hashElement, &dom->propValueRegistry);
}
