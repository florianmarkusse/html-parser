
#include "dom/document-comparison.h"

unsigned char compareNode(node_id doc1Node, Document *doc1, node_id doc2Node,
                          Document *doc2) {
    Node node1 = doc1->nodes[doc1Node - 1];
    Node node2 = doc2->nodes[doc2Node - 1];

    if (isText(node1.tagID)) {
    }

    return 1;
}

ComparisonStatus equals(Document *doc1, Document *doc2) {
    node_id doc1Node = doc1->first->nodeID;
    node_id doc2Node = doc2->first->nodeID;
    while (doc1Node && doc2Node) {
        if (compareNode(doc1Node, doc1, doc2Node, doc2) == 0) {
            return 0;
        }

        doc1Node = getNextNode(doc1Node, doc1);
        doc2Node = getNextNode(doc2Node, doc2);
    }

    return (!doc1Node && !doc2Node);
}
