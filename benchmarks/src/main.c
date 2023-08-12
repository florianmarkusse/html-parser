
#include <dirent.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#include "dom/document-user.h"
#include "dom/document.h"

#define INPUTS_DIR "benchmarks/inputs/"
#define TEST_FILE_1 INPUTS_DIR "my-test.html"

unsigned char parseFile(const char *fileLocation) {
    DataContainer dataContainer;
    createDataContainer(&dataContainer);
    Document doc1;
    if (createFromFile(fileLocation, &doc1, &dataContainer) !=
        DOCUMENT_SUCCESS) {
        destroyDataContainer(&dataContainer);
        return 0;
    }
    destroyDocument(&doc1);
    destroyDataContainer(&dataContainer);
    return 1;
}

void benchmark() {
    // Open the inputs directory
    DIR *dir = NULL;
    struct dirent *ent = NULL;
    if ((dir = opendir(INPUTS_DIR)) == NULL) {
        printf("Failed to open directory: %s\n", INPUTS_DIR);
        return;
    }

    // Traverse the directory and parse all the HTML files
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        char fileLocation[1024];
        snprintf(fileLocation, sizeof(fileLocation), "%s%s", INPUTS_DIR,
                 ent->d_name);
        printf("parsing %s\n", fileLocation);
        if (!parseFile(fileLocation)) {
            printf("Parsing document %s failed\n", fileLocation);
            break;
        }
    }

    closedir(dir);
}

int main() {
    struct timespec start;
    struct timespec end;
    double cpu_time_used = NAN;

    // Get the starting timestamp
    clock_gettime(CLOCK_MONOTONIC, &start);

    // Call the function or code section to benchmark
    benchmark();

    // Get the ending timestamp
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate the elapsed time in seconds with nanosecond precision
    cpu_time_used =
        (end.tv_sec - start.tv_sec) + (end.tv_nsec - start.tv_nsec) / 1e9;

    // Convert the elapsed time to milliseconds
    double cpu_time_used_ms = cpu_time_used * 1000;

    // Print the elapsed time in milliseconds
    printf("Elapsed Time: %.2f milliseconds\n", cpu_time_used_ms);

    return 0;
}
