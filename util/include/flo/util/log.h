#ifndef FLO_UTIL_LOG_H
#define FLO_UTIL_LOG_H

#ifdef __cplusplus
extern "C" {
#endif

#include "assert.h"
#include "flo/util/text/string.h"
#include "maths.h"
#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <string.h>
#include <unistd.h>

#define FLO_NEWLINE 0x01
#define FLO_FLUSH 0x02

typedef struct {
    unsigned char *buf;
    int fileDescriptor;
    ptrdiff_t len;
    ptrdiff_t cap;
} flo_WriteBuffer;

// When adding a value to this enum, also add the right ansi escape code in
// log.c
typedef enum {
    FLO_COLOR_RED,
    FLO_COLOR_GREEN,
    FLO_COLOR_YELLOW,
    FLO_COLOR_BLUE,
    FLO_COLOR_MAGENTA,
    FLO_COLOR_CYAN,
    FLO_COLOR_RESET,
    FLO_COLOR_NUMS
} flo_AnsiColor;

typedef enum { FLO_STDOUT, FLO_STDERR } flo_BufferType;

bool flo_flushBuffer(flo_WriteBuffer *buffer);
flo_WriteBuffer *flo_getWriteBuffer(flo_BufferType bufferType);

#define FLO_FLUSH_TO(bufferType) flo_flushBuffer(flo_getWriteBuffer(bufferType))

uint32_t flo_appendToBuffer(flo_String data, flo_WriteBuffer *buffer,
                            unsigned char flags);
uint32_t flo_appendToBufferMinSize(flo_String data, unsigned char minSize,
                                   flo_WriteBuffer *buffer,
                                   unsigned char flags);

uint32_t flo_appendCStr(char *data, flo_WriteBuffer *buffer,
                        unsigned char flags);

uint32_t flo_appendBool(bool data, flo_WriteBuffer *buffer,
                        unsigned char flags);

uint32_t flo_appendChar(char data, flo_WriteBuffer *buffer,
                        unsigned char flags);

uint32_t flo_appendPtr(void *data, flo_WriteBuffer *buffer,
                       unsigned char flags);

uint32_t flo_appendUint64(uint64_t data, flo_WriteBuffer *buffer,
                          unsigned char flags);
uint32_t flo_appendUint64ToBufferMinSize(uint64_t data, unsigned char minSize,
                                         flo_WriteBuffer *buffer,
                                         unsigned char flags);

uint32_t flo_appendColor(flo_AnsiColor color, flo_BufferType bufferType);
uint32_t flo_appendColorReset(flo_BufferType bufferType);

uint32_t flo_appendPtrdiff(ptrdiff_t data, flo_WriteBuffer *buffer,
                           unsigned char flags);

uint32_t flo_appendDouble(double data, flo_WriteBuffer *buffer,
                          unsigned char flags);
uint32_t flo_appendPtrDiffToBufferMinSize(ptrdiff_t data, unsigned char minSize,
                                          flo_WriteBuffer *buffer,
                                          unsigned char flags);

uint32_t flo_noAppend();

#define FLO_LOG_DATA_COMMON(data, buffer, flags)                               \
    _Generic((data),\
        flo_String:  flo_appendToBuffer, \
        char*:  flo_appendCStr, \
        void*: flo_appendPtr,\
        unsigned char*:  flo_appendCStr, \
        char:  flo_appendChar, \
        ptrdiff_t: flo_appendPtrdiff,\
        double: flo_appendDouble,\
        uint64_t: flo_appendUint64,\
        uint32_t: flo_appendUint64,\
        uint16_t: flo_appendUint64,\
        uint8_t: flo_appendUint64,\
        int: flo_appendUint64,\
        short: flo_appendUint64,\
        bool: flo_appendBool,\
        default: flo_noAppend \
    )(data, buffer, flags)

#define FLO_LOG_DATA_2(data, buffer) FLO_LOG_DATA_COMMON(data, buffer, 0)
#define FLO_LOG_DATA_X(a, b, c, d, ...) d
#define FLO_LOG_DATA(...)                                                      \
    FLO_LOG_DATA_X(__VA_ARGS__, FLO_LOG_DATA_COMMON, FLO_LOG_DATA_2)           \
    (__VA_ARGS__)

#define FLO_LOG_DATA_BUFFER_TYPE(data, bufferType, flags)                      \
    FLO_LOG_DATA_COMMON(data, flo_getWriteBuffer(bufferType), flags)

#define FLO_LOG_1(data) FLO_LOG_DATA_BUFFER_TYPE(data, FLO_STDOUT, 0)
#define FLO_LOG_2(data, bufferType)                                            \
    FLO_LOG_DATA_BUFFER_TYPE(data, bufferType, 0)
#define FLO_LOG_3(data, bufferType, flags)                                     \
    FLO_LOG_DATA_BUFFER_TYPE(data, bufferType, flags)

#define FLO_LOG_CHOOSER_IMPL_1(arg1) FLO_LOG_1(arg1)
#define FLO_LOG_CHOOSER_IMPL_2(arg1, arg2) FLO_LOG_2(arg1, arg2)
#define FLO_LOG_CHOOSER_IMPL_3(arg1, arg2, arg3) FLO_LOG_3(arg1, arg2, arg3)
#define FLO_LOG_CHOOSER(...) FLO_LOG_CHOOSER_IMPL(__VA_ARGS__, 3, 2, 1)
#define FLO_LOG_CHOOSER_IMPL(_1, _2, _3, N, ...) FLO_LOG_CHOOSER_IMPL_##N

#define FLO_LOG(...) FLO_LOG_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#define FLO_INFO(data, ...) FLO_LOG(data, FLO_STDOUT, ##__VA_ARGS__)
#define FLO_ERROR(data, ...) FLO_LOG(data, FLO_STDERR, ##__VA_ARGS__)

#define FLO_FLUSH_AFTER(bufferType)                                            \
    for (ptrdiff_t i = 0; i < 1; i = (FLO_FLUSH_TO(bufferType), 1))

#ifdef __cplusplus
}
#endif

#endif
