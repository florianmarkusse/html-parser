#ifndef TYPE_DATA_DATA_FUNCTIONS_H
#define TYPE_DATA_DATA_FUNCTIONS_H

#include "../element/elements-container.h"

DataPageStatus insertIntoPage(const void *data, size_t byteLen,
                              size_t totalPages, size_t elementIndex,
                              ElementsContainer *container);

#endif
