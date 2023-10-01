
#include "flo/html-parser/dom/registry.h"
#include "flo/html-parser/utils/memory/memory.h"

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
flo_html_addRegistration(const flo_html_indexID flo_html_indexID,
                         const flo_html_HashElement *hashElement,
                         flo_html_BasicRegistry *basicRegistry) {
    if ((basicRegistry->registry = flo_html_resizeArray(
             basicRegistry->registry, basicRegistry->len, &basicRegistry->cap,
             sizeof(flo_html_Registration),
             FLO_HTML_PROP_REGISTRATIONS_PER_PAGE)) == NULL) {
        return DOM_ERROR_MEMORY;
    }

    flo_html_Registration *registration =
        &(basicRegistry->registry[basicRegistry->len]);
    // registration->flo_html_indexID = flo_html_indexID;
    registration->hashElement.hash = hashElement->hash;
    registration->hashElement.offset = hashElement->offset;
    basicRegistry->len++;
    return DOM_SUCCESS;
}

flo_html_DomStatus
flo_html_addBoolPropRegistration(const flo_html_indexID boolPropID,
                                 const flo_html_HashElement *hashElement,
                                 flo_html_Dom *dom) {
    return flo_html_addRegistration(boolPropID, hashElement,
                                    &dom->boolPropRegistry);
}

flo_html_DomStatus
flo_html_addPropKeyRegistration(flo_html_indexID propKeyID,
                                const flo_html_HashElement *hashElement,
                                flo_html_Dom *dom) {
    return flo_html_addRegistration(propKeyID, hashElement,
                                    &dom->propKeyRegistry);
}

flo_html_DomStatus
flo_html_addPropValueRegistration(flo_html_indexID propValueID,
                                  const flo_html_HashElement *hashElement,
                                  flo_html_Dom *dom) {
    return flo_html_addRegistration(propValueID, hashElement,
                                    &dom->propValueRegistry);
}
