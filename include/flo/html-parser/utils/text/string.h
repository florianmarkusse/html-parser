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
    (flo_html_String) { NULL, 0 }
#define FLO_HTML_S(s)                                                          \
    (flo_html_String) { (unsigned char *)(s), sizeof(s) - 1 }
#define FLO_HTML_S_LEN(s, len)                                                 \
    (flo_html_String) { (unsigned char *)(s), len }
#define FLO_HTML_S_PTRS(begin, end)                                            \
    (flo_html_String) { (unsigned char *)(begin), ((end) - (begin)) }

#define FLO_HTML_S_P(string) (int)(string).len, (string).buf

typedef struct {
    unsigned char *buf;
    ptrdiff_t len;
} flo_html_String;

__attribute__((unused)) static bool flo_html_stringEquals(flo_html_String a,
                                                          flo_html_String b) {
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
__attribute__((unused)) static flo_html_String
flo_html_strcpy(flo_html_String dest, flo_html_String src) {
#ifdef DEBUG
    assert(dest.len >= src.len) printf("I AM COPYING IN DEBUG MODE\n");
#endif
    memcpy(dest.buf, src.buf, src.len);
    dest.buf[src.len] = '\0';
    dest.len = src.len;
    return dest;
}
__attribute__((unused)) static inline unsigned char
flo_html_getChar(flo_html_String str, const ptrdiff_t index) {
#ifdef DEBUG
    // Debug mode: Perform bounds checking
    assert(index >= 0 && index < str.len);
    printf("I AM GETTING A CHAR AT INDEX IN DEBUG MODE\n");
#endif

    return str.buf[index];
}
__attribute__((unused)) static inline unsigned char *
flo_html_getCharPtr(flo_html_String str, const ptrdiff_t index) {
#ifdef DEBUG
    // Debug mode: Perform bounds checking
    assert(index >= 0 && index < str.len);
    printf("I AM GETTING A CHAR PTR AT INDEX IN DEBUG MODE\n");
#endif

    return &str.buf[index];
}
__attribute__((unused)) static inline bool
flo_html_containsChar(flo_html_String s, unsigned char ch) {
    for (ptrdiff_t i = 0; i < s.len; i++) {
        if (s.buf[i] == ch) {
            return true;
        }
    }
    return false;
}
__attribute__((unused)) static inline ptrdiff_t
flo_html_firstOccurenceOfFrom(flo_html_String s, unsigned char ch,
                              ptrdiff_t from) {
#ifdef DEBUG
    // Debug mode: Perform bounds checking
    assert(from >= 0 && from < s.len);
    printf("I AM GETTING A FIRST OCC OF IN DEBUG MODE\n");
#endif

    for (ptrdiff_t i = from; i < s.len; i++) {
        if (s.buf[i] == ch) {
            return i;
        }
    }
    return -1;
}
__attribute__((unused)) static inline ptrdiff_t
flo_html_firstOccurenceOf(flo_html_String s, unsigned char ch) {
    return flo_html_firstOccurenceOfFrom(s, ch, 0);
}

#ifdef __cplusplus
}
#endif

#endif
