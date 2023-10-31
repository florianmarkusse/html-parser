#include <flo/html-parser.h>
#include <unistd.h>

__AFL_FUZZ_INIT(); // NOLINT

int main() {
    __AFL_INIT();                                 // NOLINT
    unsigned char *buf = __AFL_FUZZ_TESTCASE_BUF; // NOLINT
    while (__AFL_LOOP(10000)) {
        int len = __AFL_FUZZ_TESTCASE_LEN; // NOLINT

        flo_html_Arena arena = flo_html_newArena(1U << 27U);
        void *jmp_buf[5];
        if (__builtin_setjmp(jmp_buf)) {
            flo_html_destroyArena(&arena);
            FLO_HTML_PRINT_ERROR("OOM in arena!\n");
            return -1;
        }
        arena.jmp_buf = jmp_buf;
        flo_html_String html = (flo_html_String){.buf = buf, .len = len};

        flo_html_Dom *result = flo_html_createDom(html, &arena);
        if (result != NULL) {
            flo_html_printHTML(result);
        } else {
            return 1;
        }
    }

    return 0;
}
