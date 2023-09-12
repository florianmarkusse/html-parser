#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/type/data/data-functions.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/type/node/node.h"
#include "flo/html-parser/utils/print/error.h"

ElementStatus initStringRegistry(StringRegistry *stringRegistry,
                                 const size_t stringsCapacity,
                                 const size_t pageSize) {
    if (initStringHashSet(&stringRegistry->set, stringsCapacity) !=
        HASH_SUCCESS) {
        PRINT_ERROR("Failure initing hash set!\n");
        return ELEMENT_MEMORY;
    }

    ElementStatus result = ELEMENT_SUCCESS;
    if ((result = initElementsContainer(&stringRegistry->container,
                                        pageSize)) != ELEMENT_SUCCESS) {
        destroyStringHashSet(&stringRegistry->set);
    }

    return result;
}

void destroyStringRegistry(StringRegistry *stringRegistry) {
    destroyStringHashSet(&stringRegistry->set);
    destroyElementsContainer(&stringRegistry->container);
}

// TODO(florian): USE MORE SENSIBLE VALUES THAN TOTAL_ELEMENTS
ElementStatus createTextStore(TextStore *textStore) {
    ElementStatus result = ELEMENT_SUCCESS;
    ElementStatus currentStatus = ELEMENT_SUCCESS;

    if ((currentStatus = initStringRegistry(&textStore->tags,
                                            TOTAL_ELEMENTS, TAGS_PAGE_SIZE)) !=
        ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(currentStatus),
                             "Failed to initialize tags string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus =
             initStringRegistry(&textStore->boolProps, TOTAL_ELEMENTS,
                                BOOL_PROPS_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(
            elementStatusToString(currentStatus),
            "Failed to initialize bool props string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus =
             initStringRegistry(&textStore->propKeys, TOTAL_ELEMENTS,
                                PROP_KEYS_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(
            elementStatusToString(currentStatus),
            "Failed to initialize prop keys string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus =
             initStringRegistry(&textStore->propValues, TOTAL_ELEMENTS,
                                PROP_VALUES_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(
            elementStatusToString(currentStatus),
            "Failed to initialize prop values string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus =
             initElementsContainer(&textStore->text, TEXT_PAGE_SIZE) !=
             ELEMENT_SUCCESS)) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(currentStatus),
                             "Failed to initialize text elements container!\n");
        result = currentStatus;
    }

    if (result != ELEMENT_SUCCESS) {
        destroyTextStore(textStore);
    }

    return result;
}

void destroyTextStore(TextStore *textStore) {
    destroyStringRegistry(&textStore->tags);
    destroyStringRegistry(&textStore->boolProps);
    destroyStringRegistry(&textStore->propKeys);
    destroyStringRegistry(&textStore->propValues);
    destroyElementsContainer(&textStore->text);
}

ElementStatus elementSizeCheck(char *buffer, const size_t bufferLen,
                               const char *elementStart,
                               const size_t elementLength) {
    if (elementLength >= bufferLen) {
        PRINT_ERROR("Element is too long, size=%zu, to fit into page.\n",
                    elementLength);
        PRINT_ERROR("Printing first part of element:\n");

        memcpy(buffer, elementStart, bufferLen - 1);
        buffer[bufferLen - 1] = '\0';

        PRINT_ERROR("%s\n", buffer);
        PRINT_ERROR("max size is %zu (null terminator :))\n", bufferLen - 1);

        return ELEMENT_TOO_LONG;
    }

    return ELEMENT_SUCCESS;
}

ElementStatus createNewElement(StringRegistry *stringRegistry,
                               const char *element, HashElement *hashElement,
                               indexID *indexID) {
    // insert element into the has table.
    DataPageStatus insertStatus =
        insertIntoPageWithhash(element, strlen(element) + 1, TOTAL_PAGES,
                               stringRegistry, hashElement, indexID);
    if (insertStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(insertStatus),
                               "Could not find or create element \"%s\"",
                               element);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    return ELEMENT_CREATED;
}

/**
 * If the element is found, then  indexID is set.
 * HashElement is always set.
 */
ElementStatus elementToIndex(StringRegistry *stringRegistry,
                             const char *elementStart, size_t elementLength,
                             HashElement *hashElement, indexID *indexID) {
    char buffer[stringRegistry->container.pageSize];
    const ElementStatus sizeCheck =
        elementSizeCheck(buffer, stringRegistry->container.pageSize,
                         elementStart, elementLength);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    memcpy(buffer, elementStart, elementLength);
    buffer[elementLength] = '\0';

    if (containsStringWithDataHashSet(&stringRegistry->set, buffer, hashElement,
                                      indexID)) {
        return ELEMENT_FOUND;
    }

    return createNewElement(stringRegistry, buffer, hashElement, indexID);
}

ElementStatus insertElement(ElementsContainer *elementsContainer,
                            const char *elementStart, size_t elementLength,
                            char **dataLocation) {
    char buffer[elementsContainer->pageSize];
    const ElementStatus sizeCheck = elementSizeCheck(
        buffer, elementsContainer->pageSize, elementStart, elementLength);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    memcpy(buffer, elementStart, elementLength);
    buffer[elementLength] = '\0';

    const DataPageStatus dataPageStatus =
        insertInSuitablePage(buffer, elementLength + 1, TOTAL_PAGES,
                             elementsContainer, dataLocation);
    if (dataPageStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_ONLY(dataPageStatusToString(dataPageStatus),
                             "Failed to insert element\n");
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    return ELEMENT_CREATED;
}
