#ifndef FLO_HTML_PARSER_UTIL_PARSE_H
#define FLO_HTML_PARSER_UTIL_PARSE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "text/string.h"

typedef struct {
    flo_String text;
    ptrdiff_t idx;
} flo_parse_Status;

#define FLO_PARSE_NEXT_CHAR_WHILE_1(parseStatus, condition)                    \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_getChar((parseStatus).text, (parseStatus).idx);           \
        if (condition) {                                                       \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
        (parseStatus).idx++;                                                   \
    }

#define FLO_PARSE_NEXT_CHAR_WHILE_2(parseStatus, condition, body)              \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_getChar((parseStatus).text, (parseStatus).idx);           \
        if (condition) {                                                       \
            { body }                                                           \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
        (parseStatus).idx++;                                                   \
    }

#define FLO_PARSE_NEXT_CHAR_WHILE_X(a, b, c, d, ...) d
#define FLO_PARSE_NEXT_CHAR_WHILE(...)                                         \
    FLO_PARSE_NEXT_CHAR_WHILE_X(__VA_ARGS__, FLO_PARSE_NEXT_CHAR_WHILE_2,      \
                                FLO_PARSE_NEXT_CHAR_WHILE_1)                   \
    (__VA_ARGS__);

#define FLO_PARSE_PARSE_CHAR_WHILE(parseStatus, condition, body)               \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_getChar((parseStatus).text, (parseStatus).idx);           \
        if (condition) {                                                       \
            { body }                                                           \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
    }

#define FLO_PARSE_NEXT_CHAR_UNTIL_2(parseStatus, condition, body)              \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_getChar((parseStatus).text, (parseStatus).idx);           \
        if (!(condition)) {                                                    \
            { body }                                                           \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
        (parseStatus).idx++;                                                   \
    }

#define FLO_PARSE_NEXT_CHAR_UNTIL_1(parseStatus, condition)                    \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_getChar((parseStatus).text, (parseStatus).idx);           \
        if (!(condition)) {                                                    \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
        (parseStatus).idx++;                                                   \
    }

#define FLO_PARSE_NEXT_CHAR_UNTIL_X(a, b, c, d, ...) d
#define FLO_PARSE_NEXT_CHAR_UNTIL(...)                                         \
    FLO_PARSE_NEXT_CHAR_UNTIL_X(__VA_ARGS__, FLO_PARSE_NEXT_CHAR_UNTIL_2,      \
                                FLO_PARSE_NEXT_CHAR_UNTIL_1)                   \
    (__VA_ARGS__);

#define FLO_PARSE_PARSE_CHAR_UNTIL(parseStatus, condition, body)               \
    while ((parseStatus).idx < (parseStatus).text.len) {                       \
        unsigned char ch =                                                     \
            flo_getChar((parseStatus).text, (parseStatus).idx);           \
        if (!(condition)) {                                                    \
            { body }                                                           \
        } else {                                                               \
            break;                                                             \
        }                                                                      \
    }

#define FLO_PARSE_SKIP_EMPTY_SPACE(parseStatus)                                \
    FLO_PARSE_NEXT_CHAR_WHILE(parseStatus,                                     \
                              (ch == ' ' || flo_isFormattingCharacter(ch)))

#define FLO_PARSE_SKIP_COMMENT(parseStatus)                                    \
    ps.idx += HTML_COMMENT_START_LENGTH;                                       \
    FLO_PARSE_NEXT_CHAR_UNTIL(                                                 \
        ps,                                                                    \
        (ch == '>' &&                                                          \
         flo_stringEquals(                                                \
             FLO_STRING("--"),                                                 \
             FLO_STRING_LEN(flo_getCharPtr(ps.text, ps.idx - 2), 2))))    \
    ps.idx++;

__attribute__((unused)) static inline unsigned char
flo_parse_currentChar(flo_parse_Status ps) {
    if (ps.idx < ps.text.len) {
        return flo_getChar(ps.text, ps.idx);
    }
    return '\0';
}

#ifdef __cplusplus
}
#endif

#endif
