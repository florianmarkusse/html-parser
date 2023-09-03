#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/dom/utils.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/utils/print/error.h"

void printAttributes(const indexID tagID1, const StringHashSet *set1,
                     const Dom *dom1, const DataContainer *dataContainer1,
                     const indexID tagID2, const StringHashSet *set2,
                     const Dom *dom2, const DataContainer *dataContainer2) {
    const char *tag1 = getTag(tagID1, dom1, dataContainer1);
    PRINT_ERROR("Printing certain attributes of node 1 with tag %s:\n", tag1);

    StringHashSetIterator iterator;
    initStringHashSetIterator(&iterator, set1);

    while (hasNextStringHashSetIterator(&iterator)) {
        const char *attribute = nextStringHashSetIterator(&iterator);
        PRINT_ERROR("%s\n", attribute);
    }

    const char *tag2 = getTag(tagID2, dom2, dataContainer2);
    PRINT_ERROR("Printing certain attributes of node 2 with tag %s:\n", tag2);

    initStringHashSetIterator(&iterator, set2);

    while (hasNextStringHashSetIterator(&iterator)) {
        const char *attribute = nextStringHashSetIterator(&iterator);
        PRINT_ERROR("%s\n", attribute);
    }
}

HashStatus createPropsSet(const node_id nodeID, const Dom *dom,
                          const DataContainer *dataContainer,
                          StringHashSet *keySet, StringHashSet *valueSet) {
    HashStatus status = HASH_SUCCESS;
    if ((status =
             initStringHashSet(keySet, MAX_PROPERTIES * 2) != HASH_SUCCESS)) {
        ERROR_WITH_CODE_ONLY(hashStatusToString(status),
                             "Failed to initialize hash key set");
        return status;
    }
    if ((status =
             initStringHashSet(valueSet, MAX_PROPERTIES * 2) != HASH_SUCCESS)) {
        ERROR_WITH_CODE_ONLY(hashStatusToString(status),
                             "Failed to initialize hash value set");
        destroyStringHashSet(keySet);
        return status;
    }
    // TODO(florian): make faster. (BTREE)
    for (size_t i = 0; i < dom->propsLen; i++) {
        if (dom->props[i].nodeID == nodeID) {
            indexID keyID = dom->props[i].keyID;
            const char *propKey = getPropKey(keyID, dom, dataContainer);
            if ((status = insertStringHashSet(keySet, propKey)) !=
                HASH_SUCCESS) {
                ERROR_WITH_CODE_FORMAT(hashStatusToString(status),
                                       "Failed to insert %s into key hash set",
                                       propKey);
                destroyStringHashSet(keySet);
                destroyStringHashSet(valueSet);
                return status;
            }

            indexID valueID = dom->props[i].valueID;
            const char *propValue = getPropValue(valueID, dom, dataContainer);
            if ((status = insertStringHashSet(keySet, propValue)) !=
                HASH_SUCCESS) {
                ERROR_WITH_CODE_FORMAT(
                    hashStatusToString(status),
                    "Failed to insert %s into value hash set", propValue);
                destroyStringHashSet(keySet);
                destroyStringHashSet(valueSet);
                return status;
            }
        }
    }

    return status;
}

ComparisonStatus compareProps(const Node *node1, const Dom *dom1,
                              const DataContainer *dataContainer1,
                              const Node *node2, const Dom *dom2,
                              const DataContainer *dataContainer2,
                              const bool printDifferences) {
    HashStatus hashStatus = HASH_SUCCESS;

    StringHashSet keySet1;
    StringHashSet valueSet1;

    if ((hashStatus = createPropsSet(node1->nodeID, dom1, dataContainer1,
                                     &keySet1, &valueSet1)) != HASH_SUCCESS) {
        destroyStringHashSet(&keySet1);
        destroyStringHashSet(&valueSet1);
        ERROR_WITH_CODE_ONLY(hashStatusToString(hashStatus),
                             "Failed to create hash sets for node 1");
        return COMPARISON_MEMORY;
    }

    StringHashSet keySet2;
    StringHashSet valueSet2;

    if ((hashStatus = createPropsSet(node2->nodeID, dom2, dataContainer2,
                                     &keySet2, &valueSet2)) != HASH_SUCCESS) {
        destroyStringHashSet(&keySet1);
        destroyStringHashSet(&valueSet1);
        destroyStringHashSet(&keySet2);
        destroyStringHashSet(&valueSet2);
        ERROR_WITH_CODE_ONLY(hashStatusToString(hashStatus),
                             "Failed to create hash sets for node 2");
        return COMPARISON_MEMORY;
    }

    ComparisonStatus result = equalsStringHashSet(&keySet1, &keySet2);
    if (result != COMPARISON_SUCCESS) {
        if (printDifferences) {
            PRINT_ERROR("Nodes contain different key props\n");
            printAttributes(node1->tagID, &keySet1, dom1, dataContainer1,
                            node2->tagID, &keySet2, dom2, dataContainer2);
        }
        destroyStringHashSet(&keySet1);
        destroyStringHashSet(&valueSet1);
        destroyStringHashSet(&keySet2);
        destroyStringHashSet(&valueSet2);
        return result;
    }

    result = equalsStringHashSet(&valueSet1, &valueSet2);
    if (result != COMPARISON_SUCCESS) {
        if (printDifferences) {
            PRINT_ERROR("Nodes contain different value props\n");
            printAttributes(node1->tagID, &valueSet1, dom1, dataContainer1,
                            node2->tagID, &valueSet2, dom2, dataContainer2);
        }
        destroyStringHashSet(&keySet1);
        destroyStringHashSet(&valueSet1);
        destroyStringHashSet(&keySet2);
        destroyStringHashSet(&valueSet2);
        return result;
    }

    destroyStringHashSet(&keySet1);
    destroyStringHashSet(&valueSet1);
    destroyStringHashSet(&keySet2);
    destroyStringHashSet(&valueSet2);

    return result;
}

HashStatus createBoolPropsSet(const node_id nodeID, const Dom *dom,
                              const DataContainer *dataContainer,
                              StringHashSet *set) {
    HashStatus status = HASH_SUCCESS;
    if ((status = initStringHashSet(set, MAX_PROPERTIES * 2) != HASH_SUCCESS)) {
        ERROR_WITH_CODE_ONLY(hashStatusToString(status),
                             "Failed to initialize hash set");
        return status;
    }
    // TODO(florian): make faster. (BTREE)
    for (size_t i = 0; i < dom->boolPropsLen; i++) {
        if (dom->boolProps[i].nodeID == nodeID) {
            indexID propID = dom->boolProps[i].propID;
            const char *boolProp = getBoolProp(propID, dom, dataContainer);
            if ((status = insertStringHashSet(set, boolProp)) != HASH_SUCCESS) {
                ERROR_WITH_CODE_FORMAT(hashStatusToString(status),
                                       "Failed to insert %s into hash set",
                                       boolProp);
                destroyStringHashSet(set);
                return status;
            }
        }
    }

    return status;
}

ComparisonStatus compareBoolProps(const Node *node1, const Dom *dom1,
                                  const DataContainer *dataContainer1,
                                  const Node *node2, const Dom *dom2,
                                  const DataContainer *dataContainer2,
                                  const bool printDifferences) {
    HashStatus hashStatus = HASH_SUCCESS;
    StringHashSet set1;
    if ((hashStatus = createBoolPropsSet(node1->nodeID, dom1, dataContainer1,
                                         &set1)) != HASH_SUCCESS) {
        ERROR_WITH_CODE_ONLY(hashStatusToString(hashStatus),
                             "Failed to create hash set 1");
        return COMPARISON_MEMORY;
    }

    StringHashSet set2;
    if ((hashStatus = createBoolPropsSet(node2->nodeID, dom2, dataContainer2,
                                         &set2)) != HASH_SUCCESS) {
        destroyStringHashSet(&set1);
        ERROR_WITH_CODE_ONLY(hashStatusToString(hashStatus),
                             "Failed to create hash set 2");
        return COMPARISON_MEMORY;
    }

    ComparisonStatus result = equalsStringHashSet(&set1, &set2);

    if (printDifferences && result != COMPARISON_SUCCESS) {
        PRINT_ERROR("Nodes contain different bool props\n");
        printAttributes(node1->tagID, &set1, dom1, dataContainer1, node2->tagID,
                        &set2, dom2, dataContainer2);
    }

    destroyStringHashSet(&set1);
    destroyStringHashSet(&set2);

    return result;
}

bool tagStringEquals(const TagRegistration *tagRegistration1,
                     const DataContainer *dataContainer1,
                     const TagRegistration *tagRegistration2,
                     const DataContainer *dataContainer2) {
    const char *string1 = getStringFromHashSet(&dataContainer1->tags.set,
                                               &tagRegistration1->hashElement);
    const char *string2 = getStringFromHashSet(&dataContainer2->tags.set,
                                               &tagRegistration2->hashElement);
    return strcmp(string1, string2) == 0;
}

ComparisonStatus compareTags(const Node *node1, const Dom *dom1,
                             const DataContainer *dataContainer1,
                             const Node *node2, const Dom *dom2,
                             const DataContainer *dataContainer2,
                             const bool printDifferences) {
    if (node1->nodeType != node2->nodeType) {
        if (printDifferences) {
            const char *nodeType1 = nodeTypeToString(node1->nodeType);
            const char *nodeType2 = nodeTypeToString(node2->nodeType);
            PRINT_ERROR("Uncomparable nodes:"
                        "node 1 type: %s"
                        "node 2 type: %s\n",
                        nodeType1, nodeType2);
        }
        return COMPARISON_DIFFERENT_NODE_TYPE;
    }

    switch (node1->nodeType) {
    case NODE_TYPE_TEXT: {
        const char *text1 = node1->text;
        const char *text2 = node2->text;

        if (strcmp(text1, text2) == 0) {
            return COMPARISON_SUCCESS;
        }

        if (printDifferences) {
            PRINT_ERROR("Different text nodes\nnode 1: %s\nnode 2: %s\n", text1,
                        text2);
        }
        return COMPARISON_DIFFERENT_CONTENT;
    }
    case NODE_TYPE_DOCUMENT: {
        TagRegistration *tagRegistration1 = NULL;
        getTagRegistration(node1->tagID, dom1, &tagRegistration1);
        TagRegistration *tagRegistration2 = NULL;
        getTagRegistration(node2->tagID, dom2, &tagRegistration2);

        if (tagRegistration1->isPaired != tagRegistration2->isPaired) {
            if (printDifferences) {
                char singleNode = '1';
                if (!tagRegistration2->isPaired) {
                    singleNode++;
                }

                const char *tag1 = getStringFromHashSet(
                    &dataContainer1->tags.set, &tagRegistration1->hashElement);
                const char *tag2 = getStringFromHashSet(
                    &dataContainer2->tags.set, &tagRegistration2->hashElement);
                PRINT_ERROR(
                    "Uncomparable nodes: single node and paired node.\nFound "
                    "single node in node %c.\n"
                    "node 1 tag: %s\nnode 2 tag: %s\n",
                    singleNode, tag1, tag2);
            }
            return COMPARISON_DIFFERENT_NODE_TYPE;
        }

        if (!tagStringEquals(tagRegistration1, dataContainer1, tagRegistration2,
                             dataContainer2)) {
            if (printDifferences) {
                const char *tag1 = getTag(node1->nodeID, dom1, dataContainer1);
                const char *tag2 = getTag(node2->nodeID, dom2, dataContainer2);
                PRINT_ERROR("Nodes have different tags.\nnode 1 tag: %s\nnode "
                            "2 tag: %s\n",
                            tag1, tag2);
            }
            return COMPARISON_DIFFERENT_CONTENT;
        }

        return COMPARISON_SUCCESS;
    }
    default: {
        if (printDifferences) {
            const char *nodeType1 = nodeTypeToString(node1->nodeType);
            PRINT_ERROR("Comparison not implemented for this node type:"
                        "node type: %s\n",
                        nodeType1);
        }
        return COMPARISON_DIFFERENT_NODE_TYPE;
    }
    }
}

ComparisonStatus compareNode(node_id *currNodeID1, const Dom *dom1,
                             const DataContainer *dataContainer1,
                             node_id *currNodeID2, const Dom *dom2,
                             const DataContainer *dataContainer2) {
    Node node1 = dom1->nodes[*currNodeID1];
    Node node2 = dom2->nodes[*currNodeID2];

    ComparisonStatus result = compareTags(&node1, dom1, dataContainer1, &node2,
                                          dom2, dataContainer2, false);
    if (result != COMPARISON_SUCCESS) {
        return result;
    }

    if (node1.nodeType == NODE_TYPE_DOCUMENT) {
        result = compareBoolProps(&node1, dom1, dataContainer1, &node2, dom2,
                                  dataContainer2, false);
        if (result != COMPARISON_SUCCESS) {
            return result;
        }
        result = compareProps(&node1, dom1, dataContainer1, &node2, dom2,
                              dataContainer2, false);
        if (result != COMPARISON_SUCCESS) {
            return result;
        }
    }

    const node_id parentNodeID1 = *currNodeID1;
    const node_id parentNodeID2 = *currNodeID2;
    *currNodeID1 = getFirstChild(*currNodeID1, dom1);
    *currNodeID2 = getFirstChild(*currNodeID2, dom2);

    while (*currNodeID1 && *currNodeID2) {
        ComparisonStatus comp = compareNode(currNodeID1, dom1, dataContainer1,
                                            currNodeID2, dom2, dataContainer2);
        if (comp != COMPARISON_SUCCESS) {
            return comp;
        }

        *currNodeID1 = getNext(*currNodeID1, dom1);
        *currNodeID2 = getNext(*currNodeID2, dom2);
    }

    if (*currNodeID1 ^ *currNodeID2) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    *currNodeID1 = parentNodeID1;
    *currNodeID2 = parentNodeID2;

    return COMPARISON_SUCCESS;
}

// TODO(florian): make version without need to pass currNodeID1 and currNodeID2
ComparisonStatus equals(node_id *currNodeID1, const Dom *dom1,
                        const DataContainer *dataContainer1,
                        node_id *currNodeID2, const Dom *dom2,
                        const DataContainer *dataContainer2) {
    *currNodeID1 = dom1->firstNodeID;
    *currNodeID2 = dom2->firstNodeID;
    while (*currNodeID1 && *currNodeID2) {
        ComparisonStatus comp = compareNode(currNodeID1, dom1, dataContainer1,
                                            currNodeID2, dom2, dataContainer2);
        if (comp != COMPARISON_SUCCESS) {
            return comp;
        }

        *currNodeID1 = getNext(*currNodeID1, dom1);
        *currNodeID2 = getNext(*currNodeID2, dom2);
    }

    if (*currNodeID1 ^ *currNodeID2) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    return COMPARISON_SUCCESS;
}

void printFirstDifference(const node_id nodeID1, const Dom *dom1,
                          const DataContainer *dataContainer1,
                          const node_id nodeID2, const Dom *dom2,
                          const DataContainer *dataContainer2) {
    Node *node1 = &dom1->nodes[nodeID1];
    Node *node2 = &dom2->nodes[nodeID2];

    if (compareTags(node1, dom1, dataContainer1, node2, dom2, dataContainer2,
                    true) != COMPARISON_SUCCESS) {
        return;
    }

    if (node1->nodeType == NODE_TYPE_DOCUMENT) {
        if (compareBoolProps(node1, dom1, dataContainer1, node2, dom2,
                             dataContainer2, true) != COMPARISON_SUCCESS) {
            return;
        }
        if (compareProps(node1, dom1, dataContainer1, node2, dom2,
                         dataContainer2, true) != COMPARISON_SUCCESS) {
            return;
        }
    }

    PRINT_ERROR(
        "Could not find any differences for the supplied nodes & documents "
        "combination.\n");
}
