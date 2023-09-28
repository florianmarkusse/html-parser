#ifndef FLO_HTML_PARSER_UTILS_TEXT_STRING_H
#define FLO_HTML_PARSER_UTILS_TEXT_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>
#include <memory.h>
#include <stdbool.h>
#include <stddef.h>
#ifdef DEBUG
#include <assert.h>
#endif

#define FLO_HTML_EMPTY_STRING                                                  \
    (flo_html_String) { 0, NULL }
#define FLO_HTML_S(s)                                                          \
    (flo_html_String) { sizeof(s) - 1, (unsigned char *)(s) }
#define FLO_HTML_S_LEN(s, len)                                                 \
    (flo_html_String) { len, (unsigned char *)(s) }
#define FLO_HTML_S_PTRS(begin, end)                                            \
    (flo_html_String) { ((end) - (begin)), (unsigned char *)(begin) }

typedef struct {
    ptrdiff_t len;
    unsigned char *buf;
} flo_html_String;

bool flo_html_stringEquals(flo_html_String a, flo_html_String b) {
    if (a.len != b.len) {
        return false;
    }
    for (ptrdiff_t i = 0; i < a.len; i++) {
        int d = a.buf[i] - b.buf[i];
        if (d) {
            return false;
        }
    }
    return true;
}

flo_html_String flo_html_strcpy(flo_html_String dest, flo_html_String src) {
#ifdef DEBUG
    assert(dest.len >= src.len) printf("I AM COPYING IN DEBUG MODE\n");
#endif
    memcpy(dest.buf, src.buf, src.len);
    dest.buf[src.len] = '\0';
    dest.len = src.len;
    return dest;
}

inline unsigned char flo_html_getChar(flo_html_String str,
                                      const ptrdiff_t index) {
#ifdef DEBUG
    // Debug mode: Perform bounds checking
    assert(index >= 0 && index < str.len);
    printf("I AM GETTING A CHAR AT INDEX IN DEBUG MODE\n");
#endif

    return str.buf[index];
}

inline unsigned char *flo_html_getCharPtr(flo_html_String str,
                                          const ptrdiff_t index) {
#ifdef DEBUG
    // Debug mode: Perform bounds checking
    assert(index >= 0 && index < str.len);
    printf("I AM GETTING A CHAR PTR AT INDEX IN DEBUG MODE\n");
#endif

    return &str.buf[index];
}

#ifdef __cplusplus
}
#endif

#endif
