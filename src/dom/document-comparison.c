#include <string.h>

#include "dom/document-comparison.h"
#include "dom/document-utils.h"
#include "tokenizer/parse.h"
#include "utils/print/error.h"

void printProps(const element_id node1Tag, const element_id node1PropsLen,
                const element_id node1Keys[MAX_PROPERTIES],
                const element_id node1Values[MAX_PROPERTIES],
                const element_id node2Tag, const element_id node2PropsLen,
                const element_id node2Keys[MAX_PROPERTIES],
                const element_id node2Values[MAX_PROPERTIES]) {
    const char *tag1 = gTags.container.elements[node1Tag];
    PRINT_ERROR("Printing key-value props of node 1 with tag %s:\n", tag1);
    for (size_t i = 0; i < node1PropsLen; i++) {
        char *key = gPropKeys.container.elements[node1Keys[i]];
        char *value = gPropValues.container.elements[node1Values[i]];
        PRINT_ERROR("%s=\"%s\"\n", key, value);
    }

    const char *tag2 = gTags.container.elements[node2Tag];
    PRINT_ERROR("Printing key-value props of node 2 with tag %s:\n", tag2);
    for (size_t i = 0; i < node2PropsLen; i++) {
        char *key = gPropKeys.container.elements[node2Keys[i]];
        char *value = gPropValues.container.elements[node2Values[i]];
        PRINT_ERROR("%s=\"%s\"\n", key, value);
    }
}

ComparisonStatus compareProps(const Node *node1, const Document *doc1,
                              const Node *node2, const Document *doc2) {
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
        PRINT_ERROR("Nodes have different number of boolean properties.\nnode "
                    "1: %u\nnode 2: %u\n",
                    node1PropsLen, node2PropsLen);

        printProps(node1->tagID, node1PropsLen, node1Keys, node1Values,
                   node2->tagID, node2PropsLen, node2Keys, node2Values);

        return COMPARISON_MISSING_PROPERTIES;
    }

    for (size_t i = 0; i < node1PropsLen; i++) {
        if (node1Keys[i] != node2Keys[i] || node1Values[i] != node2Values[i]) {
            PRINT_ERROR("Nodes have different key-value properties.\n");
            printProps(node1->tagID, node1PropsLen, node1Keys, node1Values,
                       node2->tagID, node2PropsLen, node2Keys, node2Values);
            return COMPARISON_DIFFERENT_PROPERTIES;
        }
    }

    return COMPARISON_SUCCESS;
}

void printBoolProps(const element_id node1Tag, const element_id node1PropsLen,
                    const element_id node1Props[MAX_PROPERTIES],
                    const element_id node2Tag, const element_id node2PropsLen,
                    const element_id node2Props[MAX_PROPERTIES]) {
    const char *tag1 = gTags.container.elements[node1Tag];
    PRINT_ERROR("Printing bool props of node 1 with tag %s:\n", tag1);
    for (size_t i = 0; i < node1PropsLen; i++) {
        char *prop = gPropKeys.container.elements[node1Props[i]];
        PRINT_ERROR("%s\n", prop);
    }

    const char *tag2 = gTags.container.elements[node2Tag];
    PRINT_ERROR("Printing bool props of node 2 with tag %s:\n", tag2);
    for (size_t i = 0; i < node2PropsLen; i++) {
        char *prop = gPropKeys.container.elements[node2Props[i]];
        PRINT_ERROR("%s\n", prop);
    }
}

ComparisonStatus compareBoolProps(const Node *node1, const Document *doc1,
                                  const Node *node2, const Document *doc2) {
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
        PRINT_ERROR("Nodes have different number of boolean properties.\nnode "
                    "1: %u\nnode 2: %u\n",
                    node1PropsLen, node2PropsLen);

        printBoolProps(node1->tagID, node1PropsLen, node1Props, node2->tagID,
                       node2PropsLen, node2Props);

        return COMPARISON_MISSING_PROPERTIES;
    }

    for (size_t i = 0; i < node1PropsLen; i++) {
        if (node1Props[i] != node2Props[i]) {
            PRINT_ERROR("Nodes have different boolean properties.\n");
            printBoolProps(node1->tagID, node1PropsLen, node1Props,
                           node2->tagID, node2PropsLen, node2Props);
            return COMPARISON_DIFFERENT_PROPERTIES;
        }
    }

    return COMPARISON_SUCCESS;
}

ComparisonStatus compareTags(const Node *node1, const Document *doc1,
                             const Node *node2, const Document *doc2) {
    if (isText(node1->tagID) ^ isText(node2->tagID)) {
        const char *text = NULL;
        const char *tag = NULL;
        char textDoc = '1';
        if (isText(node1->tagID)) {
            text = getText(node1->nodeID, doc1);
            tag = gTags.container.elements[node2->tagID];
        } else {
            textDoc++;
            text = getText(node2->nodeID, doc2);
            tag = gTags.container.elements[node1->tagID];
        }

        PRINT_ERROR("Uncomparable nodes: text and not text node.\nFound text "
                    "in document %c.\n"
                    "text node: %s\ntag: %s\n",
                    textDoc, text, tag);
        return COMPARISON_DIFFERENT_NODE_TYPE;
    }

    if (isText(node1->tagID) & isText(node2->tagID)) {
        const char *text1 = getText(node1->nodeID, doc1);
        const char *text2 = getText(node2->nodeID, doc2);

        if (strcmp(text1, text2) == 0) {
            return COMPARISON_SUCCESS;
        }

        PRINT_ERROR("Different text nodes\nnode 1: %s\nnode 2: %s\n", text1,
                    text2);
        return COMPARISON_DIFFERENT_TEXT;
    }

    if (isSingle(node1->tagID) ^ isSingle(node2->tagID)) {
        char singleDoc = '1';
        if (isSingle(node2->tagID)) {
            singleDoc++;
        }

        const char *tag1 = gTags.container.elements[node1->tagID];
        const char *tag2 = gTags.container.elements[node2->tagID];
        PRINT_ERROR("Uncomparable nodes: single node and paired node.\nFound "
                    "single node in document %c.\n"
                    "document 1 tag: %s\ndocument 2 tag: %s\n",
                    singleDoc, tag1, tag2);
        return COMPARISON_DIFFERENT_NODE_TYPE;
    }

    if (node1->tagID != node2->tagID) {
        const char *tag1 = gTags.container.elements[node1->tagID];
        const char *tag2 = gTags.container.elements[node2->tagID];
        PRINT_ERROR("Nodes have different tags.\ndocument 1 tag: %s\ndocument "
                    "2 tag: %s\n",
                    tag1, tag2);
        return COMPARISON_DIFFERENT_TAGS;
    }

    return COMPARISON_SUCCESS;
}

ComparisonStatus compareNode(const node_id doc1Node, const Document *doc1,
                             const node_id doc2Node, const Document *doc2) {
    Node node1 = doc1->nodes[doc1Node - 1];
    Node node2 = doc2->nodes[doc2Node - 1];

    ComparisonStatus result = compareTags(&node1, doc1, &node2, doc2);
    if (result != COMPARISON_SUCCESS) {
        return result;
    }

    if (!isText(node1.tagID)) {
        result = compareBoolProps(&node1, doc1, &node2, doc2);
        if (result != COMPARISON_SUCCESS) {
            return result;
        }
        result = compareProps(&node1, doc1, &node2, doc2);
        if (result != COMPARISON_SUCCESS) {
            return result;
        }
    }

    node_id childNode1 = getFirstChild(doc1Node, doc1);
    node_id childNode2 = getFirstChild(doc2Node, doc2);

    while (childNode1 && childNode2) {
        ComparisonStatus comp = compareNode(childNode1, doc1, childNode2, doc2);
        if (comp != COMPARISON_SUCCESS) {
            return comp;
        }

        childNode1 = getNextNode(childNode1, doc1);
        childNode2 = getNextNode(childNode2, doc2);
    }

    if (childNode1 ^ childNode2) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    return COMPARISON_SUCCESS;
}

ComparisonStatus equals(const Document *doc1, const Document *doc2) {
    node_id doc1Node = doc1->first->nodeID;
    node_id doc2Node = doc2->first->nodeID;
    while (doc1Node && doc2Node) {
        ComparisonStatus comp = compareNode(doc1Node, doc1, doc2Node, doc2);
        if (comp != COMPARISON_SUCCESS) {
            return comp;
        }

        doc1Node = getNextNode(doc1Node, doc1);
        doc2Node = getNextNode(doc2Node, doc2);
    }

    if (doc1Node ^ doc2Node) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    return COMPARISON_SUCCESS;
}
