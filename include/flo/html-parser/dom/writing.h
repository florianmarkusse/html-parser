#ifndef FLO_HTML_PARSER_DOM_WRITING_H
#define FLO_HTML_PARSER_DOM_WRITING_H

#include "dom.h"

void printDomStatus(const Dom *dom, const DataContainer *dataContainer);
void printHTML(const Dom *dom, const DataContainer *dataContainer);
FileStatus writeHTMLToFile(const Dom *dom, const DataContainer *dataContainer,
                           const char *filePath);

#endif
