#ifndef FLO_HTML_PARSER_DOM_QUERY_MSI_UINT16_H
#define FLO_HTML_PARSER_DOM_QUERY_MSI_UINT16_H

#ifdef __cplusplus
extern "C" {
#endif

#define FLO_MSI_HTML_STARTING_EXPONENT 7

#include "flo/html-parser/definitions.h"
#include "flo/util/hash/msi/uint16-set.h"

bool flo_msi_html_uint16Insert(uint16_t value, flo_msi_Uint16 *index,
                               flo_Arena *perm);

void flo_msi_html_resetUint16(flo_msi_Uint16 *index);

flo_uint16_a flo_msi_html_toArray(flo_msi_Uint16 *index, flo_Arena *perm);

flo_msi_Uint16 flo_msi_html_copyUint16(flo_msi_Uint16 *index, flo_Arena *perm);

#ifdef __cplusplus
}
#endif

#endif
