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
ElementStatus createDataContainer(DataContainer *dataContainer) {
    ElementStatus result = ELEMENT_SUCCESS;
    ElementStatus currentStatus = ELEMENT_SUCCESS;

    if ((currentStatus = initStringRegistry(&dataContainer->tags,
                                            TOTAL_ELEMENTS, TAGS_PAGE_SIZE)) !=
        ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(currentStatus),
                             "Failed to initialize tags string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus =
             initStringRegistry(&dataContainer->boolProps, TOTAL_ELEMENTS,
                                BOOL_PROPS_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(
            elementStatusToString(currentStatus),
            "Failed to initialize bool props string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus =
             initStringRegistry(&dataContainer->propKeys, TOTAL_ELEMENTS,
                                PROP_KEYS_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(
            elementStatusToString(currentStatus),
            "Failed to initialize prop keys string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus =
             initStringRegistry(&dataContainer->propValues, TOTAL_ELEMENTS,
                                PROP_VALUES_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(
            elementStatusToString(currentStatus),
            "Failed to initialize prop values string registry!\n");
        result = currentStatus;
    }

    if ((currentStatus = initStringRegistry(&dataContainer->text,
                                            TOTAL_ELEMENTS, TEXT_PAGE_SIZE)) !=
        ELEMENT_SUCCESS) {
        ERROR_WITH_CODE_ONLY(elementStatusToString(currentStatus),
                             "Failed to initialize text string registry!\n");
        result = currentStatus;
    }

    if (result != ELEMENT_SUCCESS) {
        destroyDataContainer(dataContainer);
    }

    return result;
}

void destroyDataContainer(DataContainer *dataContainer) {
    destroyStringRegistry(&dataContainer->tags);
    destroyStringRegistry(&dataContainer->boolProps);
    destroyStringRegistry(&dataContainer->propKeys);
    destroyStringRegistry(&dataContainer->propValues);
    destroyStringRegistry(&dataContainer->text);
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

ElementStatus createNewElement(StringRegistry *newElements, const char *element,
                               HashElement *hashElement, indexID *indexID) {
    // insert element into the has table.
    DataPageStatus insertStatus =
        newInsertIntoPage(element, strlen(element) + 1, TOTAL_PAGES,
                          newElements, hashElement, indexID);
    if (insertStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(insertStatus),
                               "Could not find or create element \"%s\"",
                               element);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    return ELEMENT_CREATED;
}

/**
 * If the element is found, then only indexID is set. Otherwise, if a new
 * element had to be created, hashElement is also filled.
 */
// TODO(florian): remove seaarchElement parameter once text values completely
// bypass the hash
ElementStatus newElementToIndex(StringRegistry *newElements,
                                const char *elementStart, size_t elementLength,
                                HashElement *hashElement, indexID *indexID) {
    char buffer[newElements->container.pageSize];
    const ElementStatus sizeCheck = elementSizeCheck(
        buffer, newElements->container.pageSize, elementStart, elementLength);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    memcpy(buffer, elementStart, elementLength);
    buffer[elementLength] = '\0';

    if (containsStringWithDataHashSet(&newElements->set, buffer, hashElement,
                                      indexID)) {
        return ELEMENT_FOUND;
    }

    return createNewElement(newElements, buffer, hashElement, indexID);
}
