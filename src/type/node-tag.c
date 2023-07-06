#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type/data-page.h"
#include "type/node-tag.h"

Tags globalTags;

tag_id createTags() {
    globalTags.pairedTagsLen = 0;
    globalTags.singleTagsLen = 0;

    globalTags.pages[0] = createDataPage();
    if (globalTags.pages[0].start == NULL) {
        return FAILED_TO_ALLOCATE_FOR_PAGE_OF_TAGS;
    }
    globalTags.pageLen = 1;
    return 0;
}

void destroyTags() {
    for (int i = 0; i < globalTags.pageLen; i++) {
        free(globalTags.pages[i].start);
    }

    globalTags.pairedTagsLen = 0;
    globalTags.singleTagsLen = 0;

    globalTags.pageLen = 0;
}

tag_id findOrCreateTag(const char *tagName, tag_id *currentTagLen,
                       tag_id offset) {
    for (tag_id i = offset; i < offset + *currentTagLen; ++i) {
        // Check if tag already exists
        if (strcmp(globalTags.tags[i], tagName) == 0) {
            return i; // Return index of existing tag
        }
    }

    // Ensure tag fits into a page.
    size_t tagNameLength = strlen(tagName) + 1;
    if (tagNameLength > PAGE_SIZE) {
        fprintf(stderr, "Tag \"%s\" is too long for page.\n", tagName);
        fprintf(stderr, "Tag size:\t%zu\tPage size:\t%u\n", tagNameLength,
                PAGE_SIZE);
        return TAG_TOO_LONG; // Or handle the error in an appropriate
                             // way
    }

    // Find a suitable page for the new tag
    DataPage *suitablePage = NULL;
    page_id suitableIndex = globalTags.pageLen;
    for (page_id i = 0; i < globalTags.pageLen; ++i) {
        DataPage *page = &(globalTags.pages[i]);
        if (page->spaceLeft >= tagNameLength) {
            suitablePage = page;
            suitableIndex = i;
            break;
        }
    }

    // If no suitable page found, create a new page
    if (suitableIndex == globalTags.pageLen) {
        if (globalTags.pageLen < TOTAL_PAGES) {
            globalTags.pages[suitableIndex] = createDataPage();
            if (globalTags.pages[suitableIndex].start == NULL) {
                fprintf(stderr,
                        "Failed to allocate memory for new tag page.\n");
                return FAILED_TO_ALLOCATE_FOR_PAGE_OF_TAGS;
            }
            globalTags.pageLen++;
        } else {
            fprintf(stderr, "No more capacity to create new tag pages.\n");
            return NO_CAPACITY;
        }
    }

    // Duplicate the tag memory within the suitable page
    char *duplicatedTag = globalTags.pages[suitableIndex].freeSpace;
    memcpy(duplicatedTag, tagName, tagNameLength);
    globalTags.pages[suitableIndex].freeSpace += tagNameLength;
    globalTags.pages[suitableIndex].spaceLeft -= tagNameLength;

    globalTags.tags[offset + *currentTagLen] =
        duplicatedTag; // Point tags[i] to the duplicated memory
    (*currentTagLen)++;

    return offset + (*currentTagLen) - 1; // Return index of newly created tag
}

tag_id tagToIndex(const char *tagName, const unsigned char isPaired) {
    if (isPaired) {
        return findOrCreateTag(tagName, &(globalTags.pairedTagsLen), 0);
    }
    return findOrCreateTag(tagName, &(globalTags.singleTagsLen),
                           TOTAL_TAGS_MSB);
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
