#ifndef UTILS_PRINT_ERROR_H
#define UTILS_PRINT_ERROR_H

#include <stdio.h>

#ifdef ENABLE_DEBUG_MACRO
#define PRINT_ERROR(...) fprintf(stderr, __VA_ARGS__)
#else
#define PRINT_ERROR(...) ((void)0)
#endif

#define ERROR_WITH_CODE_ONLY(code, message)                                    \
    do {                                                                       \
        PRINT_ERROR("Error code: %s\n", code);                                 \
        PRINT_ERROR("%s\n\n", message);                                        \
    } while (0)

#define ERROR_WITH_CODE_FORMAT(code, format, ...)                              \
    do {                                                                       \
        PRINT_ERROR("Error code: %s\n", code);                                 \
        PRINT_ERROR(format "\n\n", __VA_ARGS__);                               \
    } while (0)

#endif
