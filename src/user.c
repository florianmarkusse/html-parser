
#include "flo/html-parser/user.h"
#include "flo/html-parser/util/error.h"
#include "flo/html-parser/util/memory.h"

flo_html_UserStatus flo_html_fromFile(const flo_html_String fileLocation,
                                      flo_html_ParsedHTML *parsed,
                                      flo_html_Arena *perm) {
    parsed->textStore = FLO_HTML_NEW(perm, flo_html_TextStore);
    *parsed->textStore = flo_html_createTextStore(perm);

    parsed->dom = FLO_HTML_NEW(perm, flo_html_Dom);

    if (!flo_html_createDomFromFile(fileLocation, parsed->dom,
                                    parsed->textStore, perm)) {
        return USER_FILE_FAIL;
    }

    return USER_SUCCESS;
}
