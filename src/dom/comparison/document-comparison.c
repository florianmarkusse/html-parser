#include <stdio.h>
#include <string.h>

#include "dom/comparison/document-comparison.h"
#include "dom/document-utils.h"
#include "parse/parse.h"
#include "utils/print/error.h"

void printProps(const element_id node1Tag, const element_id node1PropsLen,
                const element_id node1Keys[MAX_PROPERTIES],
                const element_id node1Values[MAX_PROPERTIES],
                const DataContainer *dataContainer1, const element_id node2Tag,
                const element_id node2PropsLen,
                const element_id node2Keys[MAX_PROPERTIES],
                const element_id node2Values[MAX_PROPERTIES],
                const DataContainer *dataContainer2) {
    const char *tag1 = dataContainer1->tags.container.elements[node1Tag];
    PRINT_ERROR("Printing key-value props of node 1 with tag %s:\n", tag1);
    for (size_t i = 0; i < node1PropsLen; i++) {
        char *key = dataContainer1->propKeys.container.elements[node1Keys[i]];
        char *value =
            dataContainer1->propValues.container.elements[node1Values[i]];
        PRINT_ERROR("%s=\"%s\"\n", key, value);
    }

    const char *tag2 = dataContainer2->tags.container.elements[node2Tag];
    PRINT_ERROR("Printing key-value props of node 2 with tag %s:\n", tag2);
    for (size_t i = 0; i < node2PropsLen; i++) {
        char *key = dataContainer2->propKeys.container.elements[node2Keys[i]];
        char *value =
            dataContainer2->propValues.container.elements[node2Values[i]];
        PRINT_ERROR("%s=\"%s\"\n", key, value);
    }
}

ComparisonStatus compareProps(const Node *node1, const Document *doc1,
                              const DataContainer *dataContainer1,
                              const Node *node2, const Document *doc2,
                              const DataContainer *dataContainer2,
                              const unsigned char printDifferences) {
    element_id node1Keys[MAX_PROPERTIES];
    element_id node1Values[MAX_PROPERTIES];
    element_id node1PropsLen = 0;
    for (size_t i = 0; i < doc1->propsLen; i++) {
        if (doc1->props[i].nodeID == node1->nodeID) {
            node1Keys[node1PropsLen] = doc1->props[i].keyID;
            node1Values[node1PropsLen] = doc1->props[i].valueID;
            node1PropsLen++;
        }
    }

    element_id node2Keys[MAX_PROPERTIES];
    element_id node2Values[MAX_PROPERTIES];
    element_id node2PropsLen = 0;
    for (size_t i = 0; i < doc2->propsLen; i++) {
        if (doc2->props[i].nodeID == node2->nodeID) {
            node2Keys[node2PropsLen] = doc2->props[i].keyID;
            node2Values[node2PropsLen] = doc2->props[i].valueID;
            node2PropsLen++;
        }
    }

    if (node1PropsLen != node2PropsLen) {
        if (printDifferences) {
            PRINT_ERROR(
                "Nodes have different number of key/boolean properties.\nnode "
                "1: %u\nnode 2: %u\n",
                node1PropsLen, node2PropsLen);

            printProps(node1->tagID, node1PropsLen, node1Keys, node1Values,
                       dataContainer1, node2->tagID, node2PropsLen, node2Keys,
                       node2Values, dataContainer2);
        }

        return COMPARISON_MISSING_PROPERTIES;
    }

    for (size_t i = 0; i < node1PropsLen; i++) {
        if (node1Keys[i] != node2Keys[i] || node1Values[i] != node2Values[i]) {
            if (printDifferences) {
                PRINT_ERROR("Nodes have different value properties.\n");
                printProps(node1->tagID, node1PropsLen, node1Keys, node1Values,
                           dataContainer1, node2->tagID, node2PropsLen,
                           node2Keys, node2Values, dataContainer2);
            }
            return COMPARISON_DIFFERENT_PROPERTIES;
        }
    }

    return COMPARISON_SUCCESS;
}

void printBoolProps(const element_id node1Tag, const element_id node1PropsLen,
                    const element_id node1Props[MAX_PROPERTIES],
                    const DataContainer *dataContainer1,
                    const element_id node2Tag, const element_id node2PropsLen,
                    const element_id node2Props[MAX_PROPERTIES],
                    const DataContainer *dataContainer2) {
    const char *tag1 = dataContainer1->tags.container.elements[node1Tag];
    PRINT_ERROR("Printing bool props of node 1 with tag %s:\n", tag1);
    for (size_t i = 0; i < node1PropsLen; i++) {
        char *prop = dataContainer1->propKeys.container.elements[node1Props[i]];
        PRINT_ERROR("%s\n", prop);
    }

    const char *tag2 = dataContainer2->tags.container.elements[node2Tag];
    PRINT_ERROR("Printing bool props of node 2 with tag %s:\n", tag2);
    for (size_t i = 0; i < node2PropsLen; i++) {
        char *prop = dataContainer2->propKeys.container.elements[node2Props[i]];
        PRINT_ERROR("%s\n", prop);
    }
}

ComparisonStatus compareBoolProps(const Node *node1, const Document *doc1,
                                  const DataContainer *dataContainer1,
                                  const Node *node2, const Document *doc2,
                                  const DataContainer *dataContainer2,
                                  const unsigned char printDifferences) {
    element_id node1Props[MAX_PROPERTIES];
    element_id node1PropsLen = 0;
    for (size_t i = 0; i < doc1->boolPropsLen; i++) {
        if (doc1->boolProps[i].nodeID == node1->nodeID) {
            node1Props[node1PropsLen] = doc1->boolProps[i].propID;
            node1PropsLen++;
        }
    }

    element_id node2Props[MAX_PROPERTIES];
    element_id node2PropsLen = 0;
    for (size_t i = 0; i < doc2->boolPropsLen; i++) {
        if (doc2->boolProps[i].nodeID == node2->nodeID) {
            node2Props[node2PropsLen] = doc2->boolProps[i].propID;
            node2PropsLen++;
        }
    }

    if (node1PropsLen != node2PropsLen) {
        if (printDifferences) {
            PRINT_ERROR(
                "Nodes have different number of boolean properties.\nnode "
                "1: %u\nnode 2: %u\n",
                node1PropsLen, node2PropsLen);

            printBoolProps(node1->tagID, node1PropsLen, node1Props,
                           dataContainer1, node2->tagID, node2PropsLen,
                           node2Props, dataContainer2);
        }

        return COMPARISON_MISSING_PROPERTIES;
    }

    for (size_t i = 0; i < node1PropsLen; i++) {
        if (node1Props[i] != node2Props[i]) {
            if (printDifferences) {
                PRINT_ERROR("Nodes have different boolean properties.\n");
                printBoolProps(node1->tagID, node1PropsLen, node1Props,
                               dataContainer1, node2->tagID, node2PropsLen,
                               node2Props, dataContainer2);
            }
            return COMPARISON_DIFFERENT_PROPERTIES;
        }
    }

    return COMPARISON_SUCCESS;
}

ComparisonStatus compareTags(const Node *node1, const Document *doc1,
                             const DataContainer *dataContainer1,
                             const Node *node2, const Document *doc2,
                             const DataContainer *dataContainer2,
                             const unsigned char printDifferences) {
    if (isText(node1->tagID) ^ isText(node2->tagID)) {
        if (printDifferences) {
            const char *text = NULL;
            const char *tag = NULL;
            char textNode = '1';
            if (isText(node1->tagID)) {
                text = getText(node1->nodeID, doc1, dataContainer1);
                tag = dataContainer1->tags.container.elements[node2->tagID];
            } else {
                textNode++;
                text = getText(node2->nodeID, doc2, dataContainer2);
                tag = dataContainer1->tags.container.elements[node1->tagID];
            }

            PRINT_ERROR(
                "Uncomparable nodes: text and not text node.\nFound text "
                "in node %c.\n"
                "text node: %s\ntag: %s\n",
                textNode, text, tag);
        }
        return COMPARISON_DIFFERENT_NODE_TYPE;
    }

    if (isText(node1->tagID) & isText(node2->tagID)) {
        const char *text1 = getText(node1->nodeID, doc1, dataContainer1);
        const char *text2 = getText(node2->nodeID, doc2, dataContainer2);

        if (strcmp(text1, text2) == 0) {
            return COMPARISON_SUCCESS;
        }

        if (printDifferences) {
            PRINT_ERROR("Different text nodes\nnode 1: %s\nnode 2: %s\n", text1,
                        text2);
        }
        return COMPARISON_DIFFERENT_TEXT;
    }

    if (isSingle(node1->tagID) ^ isSingle(node2->tagID)) {
        if (printDifferences) {
            char singleNode = '1';
            if (isSingle(node2->tagID)) {
                singleNode++;
            }

            const char *tag1 =
                dataContainer1->tags.container.elements[node1->tagID];
            const char *tag2 =
                dataContainer2->tags.container.elements[node2->tagID];
            PRINT_ERROR(
                "Uncomparable nodes: single node and paired node.\nFound "
                "single node in node %c.\n"
                "node 1 tag: %s\nnode 2 tag: %s\n",
                singleNode, tag1, tag2);
            return COMPARISON_DIFFERENT_NODE_TYPE;
        }
    }

    if (node1->tagID != node2->tagID) {
        if (printDifferences) {
            const char *tag1 =
                dataContainer1->tags.container.elements[node1->tagID];
            const char *tag2 =
                dataContainer2->tags.container.elements[node2->tagID];
            PRINT_ERROR("Nodes have different tags.\nnode 1 tag: %s\nnode "
                        "2 tag: %s\n",
                        tag1, tag2);
        }
        return COMPARISON_DIFFERENT_TAGS;
    }

    return COMPARISON_SUCCESS;
}

ComparisonStatus compareNode(node_id *currNodeID1, const Document *doc1,
                             const DataContainer *dataContainer1,
                             node_id *currNodeID2, const Document *doc2,
                             const DataContainer *dataContainer2) {
    Node node1 = doc1->nodes[*currNodeID1];
    Node node2 = doc2->nodes[*currNodeID2];

    ComparisonStatus result = compareTags(&node1, doc1, dataContainer1, &node2,
                                          doc2, dataContainer2, 0);
    if (result != COMPARISON_SUCCESS) {
        return result;
    }

    if (!isText(node1.tagID)) {
        result = compareBoolProps(&node1, doc1, dataContainer1, &node2, doc2,
                                  dataContainer2, 0);
        if (result != COMPARISON_SUCCESS) {
            return result;
        }
        result = compareProps(&node1, doc1, dataContainer1, &node2, doc2,
                              dataContainer2, 0);
        if (result != COMPARISON_SUCCESS) {
            return result;
        }
    }

    const node_id parentNodeID1 = *currNodeID1;
    const node_id parentNodeID2 = *currNodeID2;
    *currNodeID1 = getFirstChild(*currNodeID1, doc1);
    *currNodeID2 = getFirstChild(*currNodeID2, doc2);

    while (*currNodeID1 && *currNodeID2) {
        ComparisonStatus comp = compareNode(currNodeID1, doc1, dataContainer1,
                                            currNodeID2, doc2, dataContainer2);
        if (comp != COMPARISON_SUCCESS) {
            return comp;
        }

        *currNodeID1 = getNextNode(*currNodeID1, doc1);
        *currNodeID2 = getNextNode(*currNodeID2, doc2);
    }

    if (*currNodeID1 ^ *currNodeID2) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    *currNodeID1 = parentNodeID1;
    *currNodeID2 = parentNodeID2;

    return COMPARISON_SUCCESS;
}

ComparisonStatus equals(node_id *currNodeID1, const Document *doc1,
                        const DataContainer *dataContainer1,
                        node_id *currNodeID2, const Document *doc2,
                        const DataContainer *dataContainer2) {
    *currNodeID1 = doc1->firstNodeID;
    *currNodeID2 = doc2->firstNodeID;
    while (*currNodeID1 && *currNodeID2) {
        ComparisonStatus comp = compareNode(currNodeID1, doc1, dataContainer1,
                                            currNodeID2, doc2, dataContainer2);
        if (comp != COMPARISON_SUCCESS) {
            return comp;
        }

        *currNodeID1 = getNextNode(*currNodeID1, doc1);
        *currNodeID2 = getNextNode(*currNodeID2, doc2);
    }

    if (*currNodeID1 ^ *currNodeID2) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    return COMPARISON_SUCCESS;
}

void printFirstDifference(const node_id nodeID1, const Document *doc1,
                          const DataContainer *dataContainer1,
                          const node_id nodeID2, const Document *doc2,
                          const DataContainer *dataContainer2) {
    Node *node1 = &doc1->nodes[nodeID1];
    Node *node2 = &doc2->nodes[nodeID2];

    if (compareTags(node1, doc1, dataContainer1, node2, doc2, dataContainer2,
                    1) != COMPARISON_SUCCESS) {
        return;
    }

    if (!isText(node1->tagID)) {
        if (compareBoolProps(node1, doc1, dataContainer1, node2, doc2,
                             dataContainer2, 1) != COMPARISON_SUCCESS) {
            return;
        }
        if (compareProps(node1, doc1, dataContainer1, node2, doc2,
                         dataContainer2, 1) != COMPARISON_SUCCESS) {
            return;
        }
    }

    PRINT_ERROR(
        "Could not find any differences for the supplied nodes & documents "
        "combination.\n");
}
