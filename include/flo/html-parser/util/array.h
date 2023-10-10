#ifndef FLO_HTML_PARSER_UTIL_ARRAY_H
#define FLO_HTML_PARSER_UTIL_ARRAY_H

#ifdef __cplusplus
extern "C" {
#endif

#define FLO_HTML_ARRAY(T)                                                      \
    struct {                                                                   \
        T *buf;                                                                \
        ptrdiff_t len;                                                         \
    }

#ifdef __cplusplus
}
#endif

#endif
