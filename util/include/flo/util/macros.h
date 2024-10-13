#ifndef FLO_UTIL_MACROS_H
#define FLO_UTIL_MACROS_H

#ifdef __cplusplus
extern "C" {
#endif

#define FLO_MACRO_VAR(name) _##name##_##MACRO_VAR##__LINE__

#ifdef __cplusplus
}
#endif

#endif
