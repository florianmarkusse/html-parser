#include "flo/util/log.h"

#define FLO_LOG_STD_BUFFER_LEN 1 << 10

unsigned char stdoutBuf[FLO_LOG_STD_BUFFER_LEN];
unsigned char stderrBuf[FLO_LOG_STD_BUFFER_LEN];

static flo_WriteBuffer stdoutBuffer =
    (flo_WriteBuffer){.buf = stdoutBuf,
                      .cap = FLO_LOG_STD_BUFFER_LEN,
                      .len = 0,
                      .fileDescriptor = STDOUT_FILENO};
static flo_WriteBuffer stderrBuffer =
    (flo_WriteBuffer){.buf = stderrBuf,
                      .cap = FLO_LOG_STD_BUFFER_LEN,
                      .len = 0,
                      .fileDescriptor = STDERR_FILENO};

bool flo_flushBuffer(flo_WriteBuffer *buffer) {
    for (ptrdiff_t bytesWritten = 0; bytesWritten < buffer->len;) {
        ptrdiff_t partialBytesWritten =
            write(buffer->fileDescriptor, buffer->buf + bytesWritten,
                  buffer->len - bytesWritten);
        if (partialBytesWritten < 0) {
            FLO_ASSERT(false);
            return false;
        } else {
            bytesWritten += partialBytesWritten;
        }
    }

    buffer->len = 0;

    return true;
}

flo_WriteBuffer *flo_getWriteBuffer(flo_BufferType bufferType) {
    if (bufferType == FLO_STDOUT) {
        return &stdoutBuffer;
    }
    return &stderrBuffer;
}

uint32_t flo_appendRepeatedCharToBuffer(unsigned char ch, ptrdiff_t repeat,
                                        flo_WriteBuffer *buffer,
                                        unsigned char flags) {
    for (ptrdiff_t bytesWritten = 0; bytesWritten < repeat;) {
        ptrdiff_t spaceInBuffer = (buffer->cap) - buffer->len;
        ptrdiff_t dataToWrite = repeat - bytesWritten;
        ptrdiff_t bytesToWrite = FLO_MIN(spaceInBuffer, dataToWrite);
        memset(buffer->buf + buffer->len, ch, bytesToWrite);
        buffer->len += bytesToWrite;
        bytesWritten += bytesToWrite;
        if (bytesWritten < repeat) {
            flo_flushBuffer(buffer);
        }
    }

    if (flags & FLO_NEWLINE) {
        if (buffer->len >= buffer->cap) {
            flo_flushBuffer(buffer);
        }
        buffer->buf[buffer->len] = '\n';
        buffer->len++;
    }

    if (flags & FLO_FLUSH) {
        flo_flushBuffer(buffer);
    }

    return true;
}

uint32_t flo_appendToBuffer(flo_String data, flo_WriteBuffer *buffer,
                            unsigned char flags) {
    for (ptrdiff_t bytesWritten = 0; bytesWritten < data.len;) {
        // the minimum of size remaining and what is left in the buffer.
        ptrdiff_t spaceInBuffer = (buffer->cap) - buffer->len;
        ptrdiff_t dataToWrite = data.len - bytesWritten;
        ptrdiff_t bytesToWrite = FLO_MIN(spaceInBuffer, dataToWrite);
        memcpy(buffer->buf + buffer->len, data.buf + bytesWritten,
               bytesToWrite);
        buffer->len += bytesToWrite;
        bytesWritten += bytesToWrite;
        if (bytesWritten < data.len) {
            flo_flushBuffer(buffer);
        }
    }

    if (flags & FLO_NEWLINE) {
        if (buffer->len >= buffer->cap) {
            flo_flushBuffer(buffer);
        }
        buffer->buf[buffer->len] = '\n';
        buffer->len++;
    }

    if (flags & FLO_FLUSH) {
        flo_flushBuffer(buffer);
    }

    return (uint32_t)data.len;
}

uint32_t flo_appendToBufferMinSize(flo_String data, unsigned char minSize,
                                   flo_WriteBuffer *buffer,
                                   unsigned char flags) {
    // Watch the flags here.
    uint32_t written = flo_appendToBuffer(data, buffer, 0);
    if (written >= minSize) {
        flo_appendToBuffer(FLO_EMPTY_STRING, buffer, flags);
        return written;
    }

    uint32_t extraSpace = minSize - written;
    return flo_appendRepeatedCharToBuffer(' ', extraSpace, buffer, flags);
}

static flo_String flo_ansiColorToCode[FLO_COLOR_NUMS] = {
    FLO_STRING("\x1b[31m"), FLO_STRING("\x1b[32m"), FLO_STRING("\x1b[33m"),
    FLO_STRING("\x1b[34m"), FLO_STRING("\x1b[35m"), FLO_STRING("\x1b[36m"),
    FLO_STRING("\x1b[0m"),
};

uint32_t flo_appendColor(flo_AnsiColor color, flo_BufferType bufferType) {
    flo_WriteBuffer *buffer = flo_getWriteBuffer(bufferType);
    return flo_appendToBuffer(isatty(buffer->fileDescriptor)
                                  ? flo_ansiColorToCode[color]
                                  : FLO_EMPTY_STRING,
                              buffer, 0);
}
uint32_t flo_appendColorReset(flo_BufferType bufferType) {
    flo_WriteBuffer *buffer = flo_getWriteBuffer(bufferType);
    return flo_appendToBuffer(isatty(buffer->fileDescriptor)
                                  ? flo_ansiColorToCode[FLO_COLOR_RESET]
                                  : FLO_EMPTY_STRING,
                              buffer, 0);
}

uint32_t flo_appendUint64ToBufferMinSize(uint64_t data, unsigned char minSize,
                                         flo_WriteBuffer *buffer,
                                         unsigned char flags) {
    // Watch the flags here.
    uint32_t written = flo_appendUint64(data, buffer, 0);
    if (written >= minSize) {
        flo_appendToBuffer(FLO_EMPTY_STRING, buffer, flags);
        return written;
    }

    uint32_t extraSpace = minSize - written;
    return flo_appendRepeatedCharToBuffer(' ', extraSpace, buffer, flags);
}

uint32_t flo_appendCStr(char *data, flo_WriteBuffer *buffer,
                        unsigned char flags) {
    if (data == NULL) {
        return 0;
    }
    ptrdiff_t len = strlen(data);
    return flo_appendToBuffer(
        (flo_String){.buf = (unsigned char *)data, .len = len}, buffer, flags);
}

uint32_t flo_appendBool(bool data, flo_WriteBuffer *buffer,
                        unsigned char flags) {
    return flo_appendToBuffer(data ? FLO_STRING("true") : FLO_STRING("false"),
                              buffer, flags);
}

uint32_t flo_appendChar(char data, flo_WriteBuffer *buffer,
                        unsigned char flags) {
    return flo_appendToBuffer(
        (flo_String){.len = 1, .buf = (unsigned char *)&data}, buffer, flags);
}

uint32_t flo_appendPtr(void *data, flo_WriteBuffer *buffer,
                       unsigned char flags) {
    unsigned char tmp[64];
    tmp[0] = '0';
    tmp[1] = 'x';

    ptrdiff_t counter = 2;
    uintptr_t u = (uintptr_t)data;
    for (int i = 2 * sizeof(u) - 1; i >= 0; i--) {
        tmp[counter++] = "0123456789abcdef"[(u >> (4 * i)) & 15];
    }

    return flo_appendToBuffer((flo_String){.len = counter - 1, .buf = tmp},
                              buffer, flags);
}

uint32_t flo_appendUint64(uint64_t data, flo_WriteBuffer *buffer,
                          unsigned char flags) {
    unsigned char tmp[64];
    unsigned char *end = tmp + sizeof(tmp);
    unsigned char *beg = end;
    do {
        *--beg = '0' + (unsigned char)(data % 10);
    } while (data /= 10);
    return flo_appendToBuffer(FLO_STRING_PTRS(beg, end), buffer, flags);
}

uint32_t flo_appendPtrdiff(ptrdiff_t data, flo_WriteBuffer *buffer,
                           unsigned char flags) {
    unsigned char tmp[64];
    unsigned char *end = tmp + sizeof(tmp);
    unsigned char *beg = end;
    ptrdiff_t t = data > 0 ? -data : data;
    do {
        *--beg = '0' - (unsigned char)(t % 10);
    } while (t /= 10);
    if (data < 0) {
        *--beg = '-';
    }
    return flo_appendToBuffer(FLO_STRING_PTRS(beg, end), buffer, flags);
}

uint32_t flo_appendDouble(double data, flo_WriteBuffer *buffer,
                          unsigned char flags) {
    uint32_t written = 0;
    uint32_t prec = 1000000; // i.e. 6 decimals

    if (data < 0) {
        written += flo_appendChar('-', buffer, 0);
        data = -data;
    }

    data += 0.5 / ((double)prec);      // round last decimal
    if (data >= (double)(-1UL >> 1)) { // out of long range?
        return written + flo_appendToBuffer(FLO_STRING("inf"), buffer, flags);
    }

    uint64_t integral = (uint64_t)data;
    uint64_t fractional = (uint64_t)((data - (double)integral) * (double)prec);

    written += flo_appendUint64(integral, buffer, 0);
    written += flo_appendChar('.', buffer, 0);

    unsigned char counter = 0;
    for (uint32_t i = prec / 10; i > 1; i /= 10) {
        if (i > fractional) {
            counter++;
        }
    }
    written += flo_appendRepeatedCharToBuffer('0', counter, buffer, 0);

    return written + flo_appendUint64(fractional, buffer, flags);
}

uint32_t flo_appendPtrDiffToBufferMinSize(ptrdiff_t data, unsigned char minSize,
                                          flo_WriteBuffer *buffer,
                                          unsigned char flags) {
    // Watch the flags here.
    uint32_t written = flo_appendPtrdiff(data, buffer, 0);
    if (written >= minSize) {
        flo_appendToBuffer(FLO_EMPTY_STRING, buffer, flags);
        return written;
    }

    uint32_t extraSpace = minSize - written;
    return flo_appendRepeatedCharToBuffer(' ', extraSpace, buffer, flags);
}

uint32_t flo_noAppend() {
    FLO_ASSERT(false);
    return 0;
}
