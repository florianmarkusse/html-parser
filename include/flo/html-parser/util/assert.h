#ifndef FLO_HTML_PARSER_UTIL_ASSERT_H
#define FLO_HTML_PARSER_UTIL_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
#if _MSC_VER
#define ASSERT(c)                                                              \
    if (!(c))                                                                  \
        __debugbreak();
#elif __GNUC__
#define ASSERT(c)                                                              \
    if (!(c))                                                                  \
        __builtin_trap();
#else
#define ASSERT(c)                                                              \
    if (!(c))                                                                  \
        *(volatile int *)0 = 0;
#endif

#ifdef __cplusplus
}
#endif

#endif

#endif
