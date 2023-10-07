#ifndef FLO_HTML_PARSER_UTIL_TEXT_STRING_H
#define FLO_HTML_PARSER_UTIL_TEXT_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>
#include <memory.h>
#include <stdbool.h>
#include <stddef.h>

#include "flo/html-parser/util/assert.h"

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

__attribute__((unused)) static inline bool
flo_html_stringEquals(flo_html_String a, flo_html_String b) {
    return a.len == b.len && !memcmp(a.buf, b.buf, a.len);
}
__attribute__((unused)) static inline flo_html_String
flo_html_strcpy(flo_html_String dest, flo_html_String src) {
#ifdef DEBUG
    ASSERT(dest.len >= src.len);
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
    ASSERT(index >= 0 && index <= str.len);
#endif

    return str.buf[index];
}
__attribute__((unused)) static inline unsigned char *
flo_html_getCharPtr(flo_html_String str, const ptrdiff_t index) {
#ifdef DEBUG
    // Debug mode: Perform bounds checking
    ASSERT(index >= 0 && index <= str.len);
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
    ASSERT(from >= 0 && from <= s.len);
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
