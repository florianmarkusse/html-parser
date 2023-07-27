#ifndef DOM_DOCUMENT_COMPARISON_H
#define DOM_DOCUMENT_COMPARISON_H

#include "document-comparison-status.h"
#include "document.h"

ComparisonStatus equals(const Document *doc1, node_id *currNodeID1,
                        const Document *doc2, node_id *currNodeID2);
void printFirstDifference(node_id nodeID1, const Document *doc1,
                          node_id nodeID2, const Document *doc2);

#endif
