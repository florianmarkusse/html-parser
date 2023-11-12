#include <dirent.h>
#include <flo/html-parser.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <sys/mman.h>
#include <time.h>

#define INPUTS_DIR "benchmarks/inputs/"
#define CAP 1 << 27

bool parseFile(flo_String fileLocation, flo_Arena scratch) {
    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &scratch);
    if (dom == NULL) {
        return false;
    }

    return true;
}

flo_Arena setupArena() {
    char *start = mmap(NULL, CAP, PROT_READ | PROT_WRITE,
                       MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
    if (start == MAP_FAILED) {
        FLO_PRINT_ERROR("Failed to allocate memory!\n");
        return (flo_Arena){0};
    }

    flo_Arena arena = flo_createArena(start, CAP);

    void *jmp_buf[5];
    if (__builtin_setjmp(jmp_buf)) {
        if (munmap(arena.beg, arena.cap) == -1) {
            FLO_PRINT_ERROR("Failed to unmap memory from arena!\n"
                            "Arena Details:\n"
                            "  beg: %p\n"
                            "  end: %p\n"
                            "  cap: %td\n"
                            "Zeroing Arena regardless.",
                            arena.beg, arena.end, arena.cap);
        }
        FLO_PRINT_ERROR("OOM/overflow in arena!\n");
        return (flo_Arena){0};
    }
    arena.jmp_buf = jmp_buf;

    return arena;
}

void benchmark(flo_Arena scratch) {
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
        if (!parseFile(FLO_STRING_LEN(fileLocation, strlen(fileLocation)),
                       scratch)) {
            printf("Parsing DOM %s failed\n", fileLocation);
            break;
        }
        files++;
    }
    printf("parsed %zu files\n", files);

    closedir(dir);
}

int main() {
    struct timespec start;
    struct timespec end;
    double cpu_time_used = (double)NAN;

    // Get the starting timestamp
    clock_gettime(CLOCK_MONOTONIC, &start);

    flo_Arena arena = setupArena();
    if (arena.beg == NULL) {
        return -1;
    }

    // Call the function or code section to benchmark
    benchmark(arena);

    // Get the ending timestamp
    clock_gettime(CLOCK_MONOTONIC, &end);

    // Calculate the elapsed time in seconds with nanosecond precision
    cpu_time_used = (double)(end.tv_sec - start.tv_sec) +
                    (double)(end.tv_nsec - start.tv_nsec) / 1e9;

    // Convert the elapsed time to milliseconds
    double cpu_time_used_ms = cpu_time_used * 1000;

    // Print the elapsed time in milliseconds
    printf("Elapsed Time: %.2f milliseconds\n", cpu_time_used_ms);

    return 0;
}
