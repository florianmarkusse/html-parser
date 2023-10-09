
#include "flo/html-parser/user.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

#define MAX_MEMORY_PER_HTML 1U << 28U // 256MiB

flo_html_UserStatus flo_html_fromFile(const flo_html_String fileLocation,
                                      flo_html_ParsedHTML *parsed) {
    if (parsed->arena.beg == NULL) {
        parsed->arena = flo_html_newArena(MAX_MEMORY_PER_HTML);
        if (parsed->arena.beg == NULL) {
            FLO_HTML_PRINT_ERROR("Failed to create new arena!");
            return USER_MEMORY;
        }

        void *jmp_buf[5];
        if (__builtin_setjmp(jmp_buf)) {
            flo_html_destroyArena(&parsed->arena);
            FLO_HTML_PRINT_ERROR("OOM in arena!");
            return USER_MEMORY;
        }
        parsed->arena.jmp_buf = jmp_buf;
    }

    flo_html_createTextStore(&parsed->textStore, &parsed->arena);
    flo_html_createDomFromFile(fileLocation, &parsed->dom, &parsed->textStore,
                               &parsed->arena);

    return USER_SUCCESS;
}

void flo_html_destroyHTML(flo_html_ParsedHTML *parsed) {
    flo_html_destroyArena(&parsed->arena);
}
