#include <dirent.h>
#include <flo/html-parser.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

#define INPUTS_DIR "benchmarks/inputs/"

bool parseFile(flo_html_String fileLocation, flo_html_Arena scratch) {
    flo_html_ParsedHTML parsed;
    if (flo_html_fromFile(fileLocation, &parsed, &scratch) != USER_SUCCESS) {
        return false;
    }

    return true;
}

void benchmark() {
    flo_html_Arena arena = flo_html_newArena(1U << 27U);
    void *jmp_buf[5];
    if (__builtin_setjmp(jmp_buf)) {
        flo_html_destroyArena(&arena);
        FLO_HTML_PRINT_ERROR("OOM in arena!\n");
        return;
    }
    arena.jmp_buf = jmp_buf;

    // Open the inputs directory
    DIR *dir = NULL;
    struct dirent *ent = NULL;
    if ((dir = opendir(INPUTS_DIR)) == NULL) {
        printf("Failed to open directory: %s\n", INPUTS_DIR);
        return;
    }

    // Traverse the directory and parse all the HTML files
    ptrdiff_t files = 0;
    while ((ent = readdir(dir)) != NULL) {
        if (strcmp(ent->d_name, ".") == 0 || strcmp(ent->d_name, "..") == 0) {
            continue;
        }
        char fileLocation[1024];
        snprintf(fileLocation, sizeof(fileLocation), "%s%s", INPUTS_DIR,
                 ent->d_name);
        printf("parsing %s\n", fileLocation);
        if (!parseFile(FLO_HTML_S_LEN(fileLocation, strlen(fileLocation)),
                       arena)) {
            printf("Parsing DOM %s failed\n", fileLocation);
            break;
        }
        files++;
    }
    printf("parsed %zu files\n", files);

    closedir(dir);

    flo_html_destroyArena(&arena);
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
