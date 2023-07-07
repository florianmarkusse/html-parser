#include <stdlib.h>
#include <string.h>

#include "type/data-page.h"
#include "type/tag.h"
#include "utils/print/error.h"

Tags globalTags;

TagStatus createTags() {
    globalTags.pairedTagsLen = 0;
    globalTags.singleTagsLen = 0;
    globalTags.pageLen = 0;
    return TAG_SUCCESS;
}

void destroyTags() {
    for (int i = 0; i < globalTags.pageLen; i++) {
        free(globalTags.pages[i].start);
    }

    globalTags.pairedTagsLen = 0;
    globalTags.singleTagsLen = 0;

    globalTags.pageLen = 0;
}

TagStatus findOrCreateTag(const char *tagName, tag_id *currentTagLen,
                          tag_id offset, tag_id *tagID) {
    for (tag_id i = offset; i < offset + *currentTagLen; ++i) {
        // Check if tag already exists
        if (strcmp(globalTags.tags[i], tagName) == 0) {
            *tagID = i;
            return TAG_SUCCESS;
        }
    }

    char *address = NULL;
    DataPageStatus dataPageStatus =
        insertIntoPage(tagName, strlen(tagName) + 1, globalTags.pages,
                       TAGS_TOTAL_PAGES, &globalTags.pageLen, (void *)&address);
    if (dataPageStatus != DATA_PAGE_SUCCESS) {
        ERROR_WITH_CODE_FORMAT(dataPageStatusToString(dataPageStatus),
                               "Could not find or create tag \"%s\"", tagName);
        return TAG_NOT_FOUND_OR_CREATED;
    }

    globalTags.tags[offset + *currentTagLen] = address;
    (*currentTagLen)++;

    *tagID = offset + (*currentTagLen) - 1;
    return TAG_SUCCESS;
}

TagStatus tagToIndex(const char *tagStart, const size_t tagLength,
                     const unsigned char isPaired, tag_id *tagID) {
    char buffer[PAGE_SIZE];

    if (tagLength >= PAGE_SIZE) {
        PRINT_ERROR("Tag is too long to fit into page.\n");
        PRINT_ERROR("Printing first part of tag:\n");

        memcpy(buffer, tagStart, PAGE_SIZE - 1);
        buffer[PAGE_SIZE - 1] = '\0';

        PRINT_ERROR("%s\n", buffer);
        PRINT_ERROR("max size is %u\n", PAGE_SIZE);

        return TAG_TOO_LONG;
    }

    memcpy(buffer, tagStart, tagLength);
    buffer[tagLength] = '\0';

    if (isPaired) {
        return findOrCreateTag(buffer, &(globalTags.pairedTagsLen), 0, tagID);
    }
    return findOrCreateTag(buffer, &(globalTags.singleTagsLen), TOTAL_TAGS_MSB,
                           tagID);
}

unsigned char isSelfClosing(tag_id index) {
    return index >> (TOTAL_TAGS_NUM_BITS - 1) != 0;
}

void printTagStatus() {
    printf("Printing global tag status...\n\n");

    printf("Paired tags...\n");
    printf("Paired tags length:\t%hu\n", globalTags.pairedTagsLen);
    for (size_t i = 0; i < globalTags.pairedTagsLen; i++) {
        printf("i:\t%zu\tTag:\t%s\n", i, globalTags.tags[i]);
    }
    printf("\n\n");

    printf("Single tags...\n");
    printf("single tags length:\t%hu\n", globalTags.singleTagsLen);
    for (size_t i = TOTAL_TAGS_MSB;
         i < TOTAL_TAGS_MSB + globalTags.singleTagsLen; i++) {
        printf("i:\t%zu\tTag:\t%s\n", i, globalTags.tags[i]);
    }
    printf("\n\n");

    printf("Pages...\n");
    printf("Pages length:\t%hhu\n", globalTags.pageLen);
    for (size_t i = 0; i < globalTags.pageLen; i++) {
        printf("Space left:\t%hu\n", globalTags.pages[i].spaceLeft);
        printf("%.*s\n", PAGE_SIZE, globalTags.pages[i].start);

        int printedChars = 0;
        char *copy = globalTags.pages[i].start;
        while (printedChars < PAGE_SIZE) {
            if (*copy == '\0') {
                printf("~");
            } else {
                printf("%c", *copy);
            }
            copy++;
            printedChars++;
        }
        printf("\n\n");
    }
    printf("\n\n");
}
