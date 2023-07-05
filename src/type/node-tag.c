#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "type/node-tag.h"

TagPage createTagPage() {
    TagPage tagPage;
    tagPage.start = malloc(PAGE_SIZE);
    tagPage.freeSpace = tagPage.start;
    tagPage.spaceLeft = PAGE_SIZE;

    if (tagPage.start == NULL) {
        fprintf(stderr, "Failed to allocate memory for tag page.\n");
    }

    return tagPage;
}

Tags createTags() {
    Tags tags;
    tags.pairedTagsLen = 0;
    tags.singleTagsLen = 0;

    tags.pages[0] = createTagPage();
    tags.pageLen = 1;

    return tags;
}

void destroyTags(Tags *tags) {
    for (int i = 0; i < tags->pageLen; i++) {
        free(tags->pages[i].start);
    }

    tags->pairedTagsLen = 0;
    tags->singleTagsLen = 0;

    tags->pageLen = 0;
}

size_t findOrCreateTag(Tags *tags, const char *tagName, size_t *currentTagLen,
                       size_t offset) {
    for (size_t i = offset; i < offset + *currentTagLen; ++i) {
        // Check if tag already exists
        if (strcmp(tags->tags[i], tagName) == 0) {
            return i; // Return index of existing tag
        }
    }

    // Ensure tag fits into a page.
    size_t tagNameLength = strlen(tagName) + 1;
    if (tagNameLength > PAGE_SIZE) {
        fprintf(stderr, "Tag \"%s\" is too long for page.\n", tagName);
        fprintf(stderr, "Tag size:\t%zu\tPage size:\t%u\n", tagNameLength,
                PAGE_SIZE);
        return UNKNOWN_TAG; // Or handle the error in an appropriate
                            // way
    }

    // Find a suitable page for the new tag
    TagPage *suitablePage = NULL;
    for (size_t i = 0; i < tags->pageLen; ++i) {
        TagPage *page = &(tags->pages[i]);
        if (page->spaceLeft >= tagNameLength) {
            suitablePage = page;
            break;
        }
    }

    // If no suitable page found, create a new page
    if (suitablePage == NULL) {
        if (tags->pageLen < TOTAL_PAGES) {
            suitablePage = &(tags->pages[tags->pageLen]);
            suitablePage->start = malloc(PAGE_SIZE);
            suitablePage->freeSpace = suitablePage->start;
            suitablePage->spaceLeft = PAGE_SIZE;

            if (suitablePage->start == NULL) {
                fprintf(stderr,
                        "Failed to allocate memory for new tag page.\n");
                return UNKNOWN_TAG; // Or handle the error in an appropriate
                                    // way
            }

            tags->pageLen++;
        } else {
            fprintf(stderr, "No more capacity to create new tag pages.\n");
            return UNKNOWN_TAG; // Or handle the error in an appropriate way
        }
    }

    // Duplicate the tag memory within the suitable page
    char *duplicatedTag = suitablePage->freeSpace;
    memcpy(duplicatedTag, tagName, tagNameLength);
    suitablePage->freeSpace += tagNameLength;
    suitablePage->spaceLeft -= tagNameLength;

    tags->tags[offset + *currentTagLen] =
        duplicatedTag; // Point tags[i] to the duplicated memory
    (*currentTagLen)++;

    return (*currentTagLen) - 1; // Return index of newly created tag
}

size_t tagToIndex(Tags *tags, const char *tagName, const char isPaired) {
    if (isPaired) {
        return findOrCreateTag(tags, tagName, &(tags->pairedTagsLen), 0);
    }
    return findOrCreateTag(tags, tagName, &(tags->singleTagsLen),
                           TOTAL_TAGS_MSB);
}
