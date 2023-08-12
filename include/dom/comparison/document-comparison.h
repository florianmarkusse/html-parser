#ifndef DOM_COMPARISON_DOCUMENT_COMPARISON_H
#define DOM_COMPARISON_DOCUMENT_COMPARISON_H

#include "document-comparison-status.h"
#include "dom/document.h"

ComparisonStatus equals(node_id *currNodeID1, const Document *doc1,
                        const DataContainer *dataContainer1,
                        node_id *currNodeID2, const Document *doc2,
                        const DataContainer *dataContainer2);
void printFirstDifference(node_id nodeID1, const Document *doc1,
                          const DataContainer *dataContainer1, node_id nodeID2,
                          const Document *doc2,
                          const DataContainer *dataContainer2);

#endif
