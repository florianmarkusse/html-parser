#ifndef FLO_HTML_PARSER_DOM_COMPARISON_DOM_COMPARISON_H
#define FLO_HTML_PARSER_DOM_COMPARISON_DOM_COMPARISON_H

#include "flo/html-parser/comparison/comparison-status.h"
#include "flo/html-parser/dom/dom.h"

ComparisonStatus equals(node_id *currNodeID1, const Dom *dom1,
                        const DataContainer *dataContainer1,
                        node_id *currNodeID2, const Dom *dom2,
                        const DataContainer *dataContainer2);
void printFirstDifference(node_id nodeID1, const Dom *dom1,
                          const DataContainer *dataContainer1, node_id nodeID2,
                          const Dom *dom2, const DataContainer *dataContainer2);

#endif
