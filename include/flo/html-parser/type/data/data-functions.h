#ifndef FLO_HTML_PARSER_TYPE_DATA_DATA_FUNCTIONS_H
#define FLO_HTML_PARSER_TYPE_DATA_DATA_FUNCTIONS_H

#include "flo/html-parser/type/element/elements-container.h"
#include "flo/html-parser/type/element/elements.h"

DataPageStatus insertIntoPage(const void *data, size_t byteLen,
                              size_t totalPages, size_t elementIndex,
                              ElementsContainer *container);

DataPageStatus newInsertIntoPage(const void *data, size_t byteLen,
                                 size_t totalPages, NewElements *newElements,
                                 HashElement *hashElement, indexID *indexID);
#endif
