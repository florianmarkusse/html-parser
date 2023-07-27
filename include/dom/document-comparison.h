#ifndef DOM_DOCUMENT_COMPARISON_H
#define DOM_DOCUMENT_COMPARISON_H

#include "document-comparison-status.h"
#include "document.h"

ComparisonStatus equals(const Document *doc1, const Document *doc2);
void printFirstDifference(const Node *node1, const Document *doc1,
                          const Node *node2, const Document *doc2);

#endif
