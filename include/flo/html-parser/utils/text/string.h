#ifndef FLO_HTML_PARSER_UTILS_TEXT_STRING_H
#define FLO_HTML_PARSER_UTILS_TEXT_STRING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <dirent.h>
#include <stdbool.h>

#include <stddef.h>

#define FLO_HTML_EMPTY_STRING                                                  \
    (flo_html_String) { NULL, 0 }
#define FLO_HTML_S(s)                                                          \
    (flo_html_String) { (unsigned char *)(s), sizeof(s) - 1 }

typedef struct {
    unsigned char *buf;
    ptrdiff_t len;
} flo_html_String;

bool flo_html_StringEquals(flo_html_String a, flo_html_String b) {
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

#ifdef __cplusplus
}
#endif

#endif
