#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "flo/html-parser/type/data/data-functions.h"
#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/type/element/elements.h"
#include "flo/html-parser/utils/print/error.h"

#define LEN_START_VALUE 1

ElementStatus createDataContainer(DataContainer *dataContainer) {
    // The Len here start at LEN_START_VALUE, because we hash the numbers and
    // use 0 as an indication that there is no value at the location yet.

    ElementStatus result = ELEMENT_SUCCESS;
    ElementStatus currentStatus = ELEMENT_SUCCESS;

    if (initStringHashSet(&dataContainer->tagNames.set, TAGS_PAGE_SIZE) !=
        HASH_SUCCESS) {
        PRINT_ERROR("Failure initing hash set!\n");
        return ELEMENT_MEMORY;
    }
    if ((currentStatus =
             initElementsContainer(&dataContainer->tagNames.container,
                                   TAGS_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        result = currentStatus;
    }

    if ((currentStatus = initElementsContainer(&dataContainer->tags.container,
                                               TAGS_PAGE_SIZE)) !=
        ELEMENT_SUCCESS) {
        result = currentStatus;
    }
    dataContainer->tags.pairedLen = LEN_START_VALUE;
    dataContainer->tags.singleLen = LEN_START_VALUE;

    if ((currentStatus =
             initElementsContainer(&dataContainer->propKeys.container,
                                   PROP_KEYS_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        result = currentStatus;
    }
    dataContainer->propKeys.pairedLen = LEN_START_VALUE;
    dataContainer->propKeys.singleLen = LEN_START_VALUE;

    if ((currentStatus =
             initElementsContainer(&dataContainer->propValues.container,
                                   PROP_VALUES_PAGE_SIZE)) != ELEMENT_SUCCESS) {
        result = currentStatus;
    }
    dataContainer->propValues.len = LEN_START_VALUE;

    if ((currentStatus = initElementsContainer(&dataContainer->text.container,
                                               TEXT_PAGE_SIZE)) !=
        ELEMENT_SUCCESS) {
        result = currentStatus;
    }
    dataContainer->text.len = LEN_START_VALUE;

    if (result != ELEMENT_SUCCESS) {
        destroyDataContainer(dataContainer);
    }

    return result;
}

void destroyDataContainer(DataContainer *dataContainer) {
    destroyStringHashSet(&dataContainer->tagNames.set);
    destroyElementsContainer(&dataContainer->tagNames.container);

    destroyElementsContainer(&dataContainer->tags.container);
    dataContainer->tags.pairedLen = 0;
    dataContainer->tags.singleLen = 0;

    destroyElementsContainer(&dataContainer->propKeys.container);
    dataContainer->propKeys.pairedLen = 0;
    dataContainer->propKeys.singleLen = 0;

    destroyElementsContainer(&dataContainer->propValues.container);
    dataContainer->propValues.len = 0;

    destroyElementsContainer(&dataContainer->text.container);
    dataContainer->text.len = 0;
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

ElementStatus createElement(ElementsContainer *container, const char *element,
                            element_id *currentElementsLen,
                            const element_id offsetMask,
                            element_id *elementID) {
    if (*currentElementsLen >= POSSIBLE_ELEMENTS) {
        PRINT_ERROR("Max size of elements array is reached: %u\n",
                    POSSIBLE_ELEMENTS);
        PRINT_ERROR("Could not insert \"%s\".\n", element);
        return ELEMENT_ARRAY_FULL;
    }

    DataPageStatus dataPageStatus =
        insertIntoPage(element, strlen(element) + 1, TOTAL_PAGES,
                       offsetMask | (*currentElementsLen), container);
    if (dataPageStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(dataPageStatus),
                               "Could not find or create element \"%s\"",
                               element);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    *elementID = (offsetMask | (*currentElementsLen));
    (*currentElementsLen)++;

    return ELEMENT_SUCCESS;
}

ElementStatus findElement(const ElementsContainer *container,
                          const element_id *currentElementLen,
                          const char *elementName, element_id offsetMask,
                          element_id *elementID) {
    for (element_id i = offsetMask + LEN_START_VALUE;
         i < (offsetMask | *currentElementLen); ++i) {
        if (strcmp(container->elements[i], elementName) == 0) {
            *elementID = i;
            return ELEMENT_SUCCESS;
        }
    }

    return ELEMENT_NOT_FOUND_OR_CREATED;
}

ElementStatus findOrCreateElement(ElementsContainer *container,
                                  const char *elementName,
                                  element_id *currentElementLen,
                                  const element_id offsetMask,
                                  element_id *elementID) {
    if (findElement(container, currentElementLen, elementName, offsetMask,
                    elementID) == ELEMENT_SUCCESS) {
        return ELEMENT_SUCCESS;
    }

    return createElement(container, elementName, currentElementLen, offsetMask,
                         elementID);
}

ElementStatus textElementToIndex(element_id *elementID) {
    *elementID = TEXT_OFFSET;
    return ELEMENT_SUCCESS;
}

ElementStatus
elementToIndex(ElementsContainer *container, element_id *currentElementLen,
               const char *elementStart, const size_t elementLength,
               const unsigned char isPaired, const unsigned char searchElements,
               element_id *elementID) {
    char buffer[container->pageSize];
    const ElementStatus sizeCheck = elementSizeCheck(
        buffer, container->pageSize, elementStart, elementLength);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    memcpy(buffer, elementStart, elementLength);
    buffer[elementLength] = '\0';

    if (searchElements) {
        return findOrCreateElement(container, buffer, currentElementLen,
                                   (isPaired ? 0 : SINGLES_OFFSET), elementID);
    }
    return createElement(container, buffer, currentElementLen,
                         (isPaired ? 0 : SINGLES_OFFSET), elementID);
}

unsigned char isSingle(const element_id index) {
    return (index >> SINGLES_MASK) != 0;
}

unsigned char isText(const element_id index) {
    return (index >> TEXT_MASK) != 0;
}

ElementStatus createNewElement(NewElements *newElements, const char *element) {
    // insert element into the has table.
    HashStatus hashStatus = HASH_SUCCESS;
    if ((hashStatus =
             insertStringHashSet(&newElements->set, element) != HASH_SUCCESS)) {
        ERROR_WITH_CODE_ONLY(hashStatusToString(hashStatus),
                             "Could not create new element");
        return ELEMENT_ARRAY_FULL;
    }

    DataPageStatus insertStatus = newInsertIntoPage(
        element, strlen(element) + 1, TOTAL_PAGES, newElements);
    if (insertStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(dataPageStatus),
                               "Could not find or create element \"%s\"",
                               element);
        return ELEMENT_NOT_FOUND_OR_CREATED;
    }

    // TODO(florian): Need to retrieve the hash STILL!!!!!
    //
    //
    //    *elementID = (offsetMask | (*currentElementsLen));
    //    (*currentElementsLen)++;
    //
    //    return ELEMENT_SUCCESS;
    //
    //

    return ELEMENT_SUCCESS;
}

ElementStatus newElementToIndex(NewElements *newElements,
                                const char *elementStart, size_t elementLength,
                                const bool isPaired,
                                const bool searchElements) {
    char buffer[newElements->container.pageSize];
    const ElementStatus sizeCheck = elementSizeCheck(
        buffer, newElements->container.pageSize, elementStart, elementLength);
    if (sizeCheck != ELEMENT_SUCCESS) {
        return sizeCheck;
    }

    memcpy(buffer, elementStart, elementLength);
    buffer[elementLength] = '\0';

    return createNewElement(newElements, buffer);
}
