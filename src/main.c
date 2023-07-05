#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

#include "dom/document.h"
#include "type/node-tag.h"
#include "utils/file/read.h"

int main() {
    // const char *htmlString = readFile("test/test.html");
    // printf("%s\n", htmlString);
    // const Document doc = createDocument(htmlString);

    // printDocument(&doc);

    // destroyDocument(&doc);

    Tags tags = createTags();

    size_t index = tagToIndex(&tags, "head", 1);
    index = tagToIndex(&tags, "body", 1);
    index = tagToIndex(&tags, "head", 1);
    index = tagToIndex(&tags, "heading", 1);
    index = tagToIndex(&tags, "article", 1);
    index = tagToIndex(&tags, "p", 1);
    index = tagToIndex(&tags, "input", 0);
    index = tagToIndex(&tags, "section", 1);
    index = tagToIndex(&tags, "div", 1);
    index = tagToIndex(&tags, "input", 0);
    index = tagToIndex(&tags, "quote", 0);
    index = tagToIndex(&tags, "reusable-content", 0);

    printf("Paired Tags...\n");
    printf("Paired Tags length:\t%zu\n", tags.pairedTagsLen);
    for (size_t i = 0; i < tags.pairedTagsLen; i++) {
        printf("i:\t%zu\tTag:\t%s\n", i, tags.tags[i]);
    }
    printf("\n\n");

    printf("Single Tags...\n");
    printf("single Tags length:\t%zu\n", tags.singleTagsLen);
    for (size_t i = TOTAL_TAGS_MSB; i < TOTAL_TAGS_MSB + tags.singleTagsLen;
         i++) {
        printf("i:\t%zu\tTag:\t%s\n", i, tags.tags[i]);
    }
    printf("\n\n");

    printf("Pages...\n");
    printf("Pages length:\t%zu\n", tags.pageLen);
    for (size_t i = 0; i < tags.pageLen; i++) {
        printf("%zu\n", tags.pages[i].spaceLeft);
        printf("%.*s\n", PAGE_SIZE, tags.pages[i].start);

        int printedChars = 0;
        char *copy = tags.pages[i].start;
        while (printedChars < PAGE_SIZE) {
            if (*copy == '\0') {
                printf("~");
            }
            printf("%c", *copy);
            copy++;
            printedChars++;
        }
        printf("\n\n");
    }
    printf("\n\n");

    destroyTags(&tags);
}
