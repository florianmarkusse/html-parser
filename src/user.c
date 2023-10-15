
#include "flo/html-parser/user.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

#define MAX_MEMORY_PER_HTML 1U << 28U // 256MiB

flo_html_UserStatus flo_html_fromFile(const flo_html_String fileLocation,
                                      flo_html_ParsedHTML *parsed,
                                      flo_html_Arena *perm) {
    if (perm->beg == NULL) {
        *perm = flo_html_newArena(MAX_MEMORY_PER_HTML);
        if (perm->beg == NULL) {
            FLO_HTML_PRINT_ERROR("Failed to create new arena!");
            parsed->textStore = NULL;
            parsed->dom = NULL;
            return USER_MEMORY;
        }

        void *jmp_buf[5];
        if (__builtin_setjmp(jmp_buf)) {
            flo_html_destroyArena(perm);
            FLO_HTML_PRINT_ERROR("OOM in arena!");
            parsed->textStore = NULL;
            parsed->dom = NULL;
            return USER_MEMORY;
        }
        perm->jmp_buf = jmp_buf;
    }

    parsed->textStore = FLO_HTML_NEW(perm, flo_html_TextStore);
    *parsed->textStore = flo_html_createTextStore(perm);

    parsed->dom = FLO_HTML_NEW(perm, flo_html_Dom);

    if (flo_html_createDomFromFile(fileLocation, parsed->dom, parsed->textStore,
                                   perm) != DOM_SUCCESS) {
        return USER_FILE_FAIL;
    }

    return USER_SUCCESS;
}
