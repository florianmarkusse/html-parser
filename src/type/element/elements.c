#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/type/data/data-functions.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/utils/print/error.h"
#include "flo/html-parser/utils/text/string.h"

flo_html_ElementStatus
flo_html_initStringRegistry(flo_html_StringRegistry *stringRegistry,
                            const size_t stringsCapacity,
                            const size_t pageSize) {
    if (flo_html_initStringHashSet(&stringRegistry->set, stringsCapacity) !=
        HASH_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Failure initing hash set!\n");
        return ELEMENT_MEMORY;
    }

    flo_html_ElementStatus result = ELEMENT_SUCCESS;
    if ((result = initflo_html_ElementsContainer(
             &stringRegistry->container, pageSize)) != ELEMENT_SUCCESS) {
        flo_html_destroyStringHashSet(&stringRegistry->set);
    }

    return result;
}

void flo_html_destroyStringRegistry(flo_html_StringRegistry *stringRegistry) {
    flo_html_destroyStringHashSet(&stringRegistry->set);
    destroyflo_html_ElementsContainer(&stringRegistry->container);
}

// TODO(florian): USE MORE SENSIBLE VALUES THAN FLO_HTML_TOTAL_ELEMENTS
flo_html_ElementStatus flo_html_createTextStore(flo_html_TextStore *textStore) {
    flo_html_ElementStatus result = ELEMENT_SUCCESS;
    flo_html_ElementStatus currentStatus = ELEMENT_SUCCESS;

    if ((currentStatus = flo_html_initStringRegistry(
             &textStore->tags, FLO_HTML_TOTAL_ELEMENTS,
             FLO_HTML_TAGS_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(currentStatus),
            "Failed to initialize tags string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus = flo_html_initStringRegistry(
             &textStore->boolProps, FLO_HTML_TOTAL_ELEMENTS,
             FLO_HTML_BOOL_PROPS_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(currentStatus),
            "Failed to initialize bool props string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus = flo_html_initStringRegistry(
             &textStore->propKeys, FLO_HTML_TOTAL_ELEMENTS,
             FLO_HTML_PROP_KEYS_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(currentStatus),
            "Failed to initialize prop keys string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus = flo_html_initStringRegistry(
             &textStore->propValues, FLO_HTML_TOTAL_ELEMENTS,
             FLO_HTML_PROP_VALUES_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(currentStatus),
            "Failed to initialize prop values string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus = initflo_html_ElementsContainer(
                             &textStore->text, FLO_HTML_TEXT_PAGE_SIZE) !=
                         ELEMENT_SUCCESS)) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_elementStatusToString(currentStatus),
            "Failed to initialize text elements container!\n");
        result = currentStatus;
    }

    if (result != ELEMENT_SUCCESS) {
        flo_html_destroyTextStore(textStore);
    }

    return result;
}

void flo_html_destroyTextStore(flo_html_TextStore *textStore) {
    flo_html_destroyStringRegistry(&textStore->tags);
    flo_html_destroyStringRegistry(&textStore->boolProps);
    flo_html_destroyStringRegistry(&textStore->propKeys);
    flo_html_destroyStringRegistry(&textStore->propValues);
    destroyflo_html_ElementsContainer(&textStore->text);
}

flo_html_ElementStatus elementSizeCheck(unsigned char *buffer,
                                        const size_t bufferLen,
                                        const flo_html_String element) {
    if (element.len >= bufferLen) {
        FLO_HTML_PRINT_ERROR(
            "Element is too long, size=%zu, to fit into page.\n", element.len);
        FLO_HTML_PRINT_ERROR("Printing first part of element:\n");

        memcpy(buffer, element.buf, bufferLen - 1);
        buffer[bufferLen - 1] = '\0';

        FLO_HTML_PRINT_ERROR("%s\n", buffer);
        FLO_HTML_PRINT_ERROR("max size is %zu (null terminator :))\n",
                             bufferLen - 1);

        return ELEMENT_TOO_LONG;
    }

    return ELEMENT_SUCCESS;
}

flo_html_ElementStatus createNewElement(flo_html_StringRegistry *stringRegistry,
                                        const flo_html_String element,
                                        flo_html_HashElement *hashElement,
                                        flo_html_indexID *flo_html_indexID) {
    // insert element into the has table.
    flo_html_DataPageStatus insertStatus = flo_html_insertIntoPageWithHash(
        element, FLO_HTML_TOTAL_PAGES, stringRegistry, hashElement,
        flo_html_indexID);
    if (insertStatus != DATA_PAGE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_FORMAT(
            flo_html_dataPageStatusToString(insertStatus),
            "Could not find or create element \"%s\"", element.buf);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    return ELEMENT_CREATED;
}

/**
 * If the element is found, then  flo_html_indexID is set.
 * flo_html_HashElement is always set.
 */
flo_html_ElementStatus flo_html_elementToIndex(
    flo_html_StringRegistry *stringRegistry, const flo_html_String element,
    flo_html_HashElement *hashElement, flo_html_indexID *flo_html_indexID) {
    unsigned char buffer[stringRegistry->container.pageSize];
    const flo_html_ElementStatus sizeCheck =
        elementSizeCheck(buffer, stringRegistry->container.pageSize, element);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    if (flo_html_containsStringWithDataHashSet(&stringRegistry->set, element,
                                               hashElement, flo_html_indexID)) {
        return ELEMENT_FOUND;
    }

    return createNewElement(stringRegistry, element, hashElement,
                            flo_html_indexID);
}

flo_html_ElementStatus
flo_html_insertElement(flo_html_ElementsContainer *elementsContainer,
                       const flo_html_String element, char **dataLocation) {
    unsigned char buffer[elementsContainer->pageSize];
    const flo_html_ElementStatus sizeCheck =
        elementSizeCheck(buffer, elementsContainer->pageSize, element);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    const flo_html_DataPageStatus dataPageStatus =
        flo_html_insertIntoSuitablePage(element, FLO_HTML_TOTAL_PAGES,
                                        elementsContainer, dataLocation);
    if (dataPageStatus != DATA_PAGE_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_dataPageStatusToString(dataPageStatus),
            "Failed to insert element\n");
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    return ELEMENT_CREATED;
}
