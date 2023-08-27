#ifndef FLO_HTML_PARSER_TYPE_DATA_DATA_FUNCTIONS_H
#define FLO_HTML_PARSER_TYPE_DATA_DATA_FUNCTIONS_H

#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/type/element/elements.h"

DataPageStatus insertInSuitablePage(const void *data, size_t byteLen,
                                    size_t totalPages,
                                    ElementsContainer *container,
                                    char **dataLocation);

DataPageStatus insertIntoPageWithhash(const void *data, size_t byteLen,
                                      size_t totalPages,
                                      StringRegistry *stringRegistry,
                                      HashElement *hashElement,
                                      indexID *indexID);
#endif
