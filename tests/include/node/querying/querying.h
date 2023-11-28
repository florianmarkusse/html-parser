#ifndef NODE_QUERYING_QUERYING_H
#define NODE_QUERYING_QUERYING_H

#ifdef __cplusplus
extern "C" {
#endif

#include <flo/html-parser.h>

void testNodeQueries(flo_Arena scratch);
void testBoolNodeQueries(flo_Arena scratch);
void testCharNodeQueries(flo_Arena scratch);
void testArrayNodeQueries(flo_Arena scratch);

#ifdef __cplusplus
}
#endif

#endif
