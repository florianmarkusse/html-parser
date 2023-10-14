
#include "flo/html-parser/user.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

#define MAX_MEMORY_PER_HTML 1U << 28U // 256MiB

flo_html_UserStatus flo_html_fromFile(const flo_html_String fileLocation,
                                      flo_html_ParsedHTML parsed,
                                      flo_html_Arena *perm) {
    if (perm->beg == NULL) {
        *perm = flo_html_newArena(MAX_MEMORY_PER_HTML);
        if (perm->beg == NULL) {
            FLO_HTML_PRINT_ERROR("Failed to create new arena!");
            return USER_MEMORY;
        }

        void *jmp_buf[5];
        if (__builtin_setjmp(jmp_buf)) {
            flo_html_destroyArena(perm);
            FLO_HTML_PRINT_ERROR("OOM in arena!");
            return USER_MEMORY;
        }
        perm->jmp_buf = jmp_buf;
    }

    *parsed.textStore = flo_html_createTextStore(perm);
    flo_html_createDomFromFile(fileLocation, &parsed.dom, &parsed.textStore,
                               perm);

    return USER_SUCCESS;
}

void flo_html_destroyHTML(flo_html_Arena *perm) { flo_html_destroyArena(perm); }
