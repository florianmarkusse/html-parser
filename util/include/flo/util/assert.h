#ifndef FLO_UTIL_ASSERT_H
#define FLO_UTIL_ASSERT_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef DEBUG
#if _MSC_VER
#define FLO_ASSERT(c)                                                          \
    if (!(c))                                                                  \
        __debugbreak();
#elif __GNUC__
#define FLO_ASSERT(c)                                                          \
    if (!(c))                                                                  \
        __builtin_trap();
#else
#define FLO_ASSERT(c)                                                          \
    if (!(c))                                                                  \
        *(volatile int *)0 = 0;
#endif
#else
#define FLO_ASSERT(c) ((void)0)
#endif

#ifdef __cplusplus
}
#endif

#endif
