#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type/data-page.h"
#include "type/tag.h"
#include "utils/print/error.h"

Tags globalTags;

TagStatus createTags() {
    globalTags.pairedTagsLen = 0;
    globalTags.singleTagsLen = 0;

    globalTags.pages[0] = createDataPage();
    if (globalTags.pages[0].start == NULL) {
        PRINT_ERROR("Failed to allocate memory for data page.\n");
        return TAG_ERROR_MEMORY;
    }
    globalTags.pageLen = 1;
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
            return i; // Return index of existing tag
        }
    }

    char *address = NULL;
    insertIntoPage(tagName, strlen(tagName) + 1, globalTags.pages,
                   &globalTags.pageLen, address);

    /*
    // Ensure tag fits into a page.
    size_t tagNameLength = strlen(tagName) + 1;
    if (tagNameLength > PAGE_SIZE) {
        PRINT_ERROR("Tag \"%s\" is too long for page.\n", tagName);
        PRINT_ERROR("Tag size:\t%zu\tPage size:\t%u\n", tagNameLength,
                    PAGE_SIZE);
        return TAG_TOO_LONG;
    }

    page_id suitableIndex = globalTags.pageLen;
    for (page_id i = 0; i < globalTags.pageLen; ++i) {
        DataPage *page = &(globalTags.pages[i]);
        if (page->spaceLeft >= tagNameLength) {
            suitableIndex = i;
            break;
        }
    }

    if (suitableIndex == globalTags.pageLen) {
        if (globalTags.pageLen < TOTAL_PAGES) {
            globalTags.pages[suitableIndex] = createDataPage();
            if (globalTags.pages[suitableIndex].start == NULL) {
                PRINT_ERROR("Failed to allocate memory for new tag page.\n");
                return TAG_ERROR_MEMORY;
            }
            globalTags.pageLen++;
        } else {
            PRINT_ERROR("No more capacity to create new tag pages.\n");
            PRINT_ERROR("All %u pages of %u bytes are full.\n", TOTAL_PAGES,
                        PAGE_SIZE);
            return TAG_NO_CAPACITY;
        }
    }

    // Duplicate the tag memory within the suitable page
    char *duplicatedTag = globalTags.pages[suitableIndex].freeSpace;
    memcpy(duplicatedTag, tagName, tagNameLength);
    globalTags.pages[suitableIndex].freeSpace += tagNameLength;
    globalTags.pages[suitableIndex].spaceLeft -= tagNameLength;
    */

    globalTags.tags[offset + *currentTagLen] = address;
    (*currentTagLen)++;

    *tagID = offset + (*currentTagLen) - 1;
    return TAG_SUCCESS;
}

TagStatus tagToIndex(const char *tagName, const unsigned char isPaired,
                     tag_id *tagID) {
    if (isPaired) {
        return findOrCreateTag(tagName, &(globalTags.pairedTagsLen), 0, tagID);
    }
    return findOrCreateTag(tagName, &(globalTags.singleTagsLen), TOTAL_TAGS_MSB,
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
