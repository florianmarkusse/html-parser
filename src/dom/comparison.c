#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/parser/parser.h"
#include "flo/html-parser/type/element/elements-print.h"
#include "flo/html-parser/util//error.h"

void printAttributes(
    const flo_html_indexID tagID1, const flo_html_StringHashSet *set1,
    const flo_html_Dom *dom1, const flo_html_TextStore *textStore1,
    const flo_html_indexID tagID2, const flo_html_StringHashSet *set2,
    const flo_html_Dom *dom2, const flo_html_TextStore *textStore2) {
    const flo_html_String tag1 = flo_html_getTag(tagID1, dom1, textStore1);
    FLO_HTML_PRINT_ERROR(
        "Printing certain attributes of node 1 with tag %.*s:\n",
        FLO_HTML_S_P(tag1));

    flo_html_StringHashSetIterator iterator;
    flo_html_initStringHashSetIterator(&iterator, set1);

    while (flo_html_hasNextStringHashSetIterator(&iterator)) {
        const flo_html_String attribute =
            flo_html_nextStringHashSetIterator(&iterator);
        FLO_HTML_PRINT_ERROR("%.*s\n", FLO_HTML_S_P(attribute));
    }

    const flo_html_String tag2 = flo_html_getTag(tagID2, dom2, textStore2);
    FLO_HTML_PRINT_ERROR(
        "Printing certain attributes of node 2 with tag %.*s:\n",
        FLO_HTML_S_P(tag2));

    flo_html_initStringHashSetIterator(&iterator, set2);

    while (flo_html_hasNextStringHashSetIterator(&iterator)) {
        const flo_html_String attribute =
            flo_html_nextStringHashSetIterator(&iterator);
        FLO_HTML_PRINT_ERROR("%.*s\n", FLO_HTML_S_P(attribute));
    }
}

flo_html_HashStatus createPropsSet(const flo_html_node_id nodeID,
                                   const flo_html_Dom *dom,
                                   const flo_html_TextStore *textStore,
                                   flo_html_StringHashSet *keySet,
                                   flo_html_StringHashSet *valueSet) {
    flo_html_HashStatus status = HASH_SUCCESS;
    if ((status = flo_html_initStringHashSet(keySet, FLO_HTML_MAX_PROPERTIES *
                                                         2) != HASH_SUCCESS)) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_hashStatusToString(status),
                                      "Failed to initialize hash key set");
        return status;
    }
    if ((status = flo_html_initStringHashSet(
                      valueSet, FLO_HTML_MAX_PROPERTIES * 2) != HASH_SUCCESS)) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_hashStatusToString(status),
                                      "Failed to initialize hash value set");
        flo_html_destroyStringHashSet(keySet);
        return status;
    }
    // TODO(florian): make faster. (BTREE)
    for (ptrdiff_t i = 0; i < dom->propsLen; i++) {
        if (dom->props[i].nodeID == nodeID) {
            flo_html_indexID keyID = dom->props[i].keyID;
            const flo_html_String propKey =
                flo_html_getPropKey(keyID, dom, textStore);
            if ((status = flo_html_insertStringHashSet(keySet, propKey)) !=
                HASH_SUCCESS) {
                FLO_HTML_ERROR_WITH_CODE_FORMAT(
                    flo_html_hashStatusToString(status),
                    "Failed to insert %.*s into key hash set",
                    FLO_HTML_S_P(propKey));
                flo_html_destroyStringHashSet(keySet);
                flo_html_destroyStringHashSet(valueSet);
                return status;
            }

            flo_html_indexID valueID = dom->props[i].valueID;
            const flo_html_String propValue =
                flo_html_getPropValue(valueID, dom, textStore);
            if ((status = flo_html_insertStringHashSet(keySet, propValue)) !=
                HASH_SUCCESS) {
                FLO_HTML_ERROR_WITH_CODE_FORMAT(
                    flo_html_hashStatusToString(status),
                    "Failed to insert %.*s into value hash set",
                    FLO_HTML_S_P(propValue));
                flo_html_destroyStringHashSet(keySet);
                flo_html_destroyStringHashSet(valueSet);
                return status;
            }
        }
    }

    return status;
}

// TODO: this function does not correct compare properties
// e.g. <a thing="a", foo="bar" /> IS NOT EQUAL TO
//      <a foo="a", thing="bar" />
flo_html_ComparisonStatus
compareProps(const flo_html_Node *node1, const flo_html_Dom *dom1,
             const flo_html_TextStore *textStore1, const flo_html_Node *node2,
             const flo_html_Dom *dom2, const flo_html_TextStore *textStore2,
             const bool printDifferences) {
    flo_html_HashStatus hashStatus = HASH_SUCCESS;

    flo_html_StringHashSet keySet1;
    flo_html_StringHashSet valueSet1;

    if ((hashStatus = createPropsSet(node1->nodeID, dom1, textStore1, &keySet1,
                                     &valueSet1)) != HASH_SUCCESS) {
        flo_html_destroyStringHashSet(&keySet1);
        flo_html_destroyStringHashSet(&valueSet1);
        FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_hashStatusToString(hashStatus),
                                      "Failed to create hash sets for node 1");
        return COMPARISON_MEMORY;
    }

    flo_html_StringHashSet keySet2;
    flo_html_StringHashSet valueSet2;

    if ((hashStatus = createPropsSet(node2->nodeID, dom2, textStore2, &keySet2,
                                     &valueSet2)) != HASH_SUCCESS) {
        flo_html_destroyStringHashSet(&keySet1);
        flo_html_destroyStringHashSet(&valueSet1);
        flo_html_destroyStringHashSet(&keySet2);
        flo_html_destroyStringHashSet(&valueSet2);
        FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_hashStatusToString(hashStatus),
                                      "Failed to create hash sets for node 2");
        return COMPARISON_MEMORY;
    }

    flo_html_ComparisonStatus result =
        flo_html_equalsStringHashSet(&keySet1, &keySet2);
    if (result != COMPARISON_SUCCESS) {
        if (printDifferences) {
            FLO_HTML_PRINT_ERROR("Nodes contain different key props\n");
            printAttributes(node1->tagID, &keySet1, dom1, textStore1,
                            node2->tagID, &keySet2, dom2, textStore2);
        }
        flo_html_destroyStringHashSet(&keySet1);
        flo_html_destroyStringHashSet(&valueSet1);
        flo_html_destroyStringHashSet(&keySet2);
        flo_html_destroyStringHashSet(&valueSet2);
        return result;
    }

    result = flo_html_equalsStringHashSet(&valueSet1, &valueSet2);
    if (result != COMPARISON_SUCCESS) {
        if (printDifferences) {
            FLO_HTML_PRINT_ERROR("Nodes contain different value props\n");
            printAttributes(node1->tagID, &valueSet1, dom1, textStore1,
                            node2->tagID, &valueSet2, dom2, textStore2);
        }
        flo_html_destroyStringHashSet(&keySet1);
        flo_html_destroyStringHashSet(&valueSet1);
        flo_html_destroyStringHashSet(&keySet2);
        flo_html_destroyStringHashSet(&valueSet2);
        return result;
    }

    flo_html_destroyStringHashSet(&keySet1);
    flo_html_destroyStringHashSet(&valueSet1);
    flo_html_destroyStringHashSet(&keySet2);
    flo_html_destroyStringHashSet(&valueSet2);

    return result;
}

flo_html_HashStatus createBoolPropsSet(const flo_html_node_id nodeID,
                                       const flo_html_Dom *dom,
                                       const flo_html_TextStore *textStore,
                                       flo_html_StringHashSet *set) {
    flo_html_HashStatus status = HASH_SUCCESS;
    if ((status = flo_html_initStringHashSet(set, FLO_HTML_MAX_PROPERTIES *
                                                      2) != HASH_SUCCESS)) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_hashStatusToString(status),
                                      "Failed to initialize hash set");
        return status;
    }
    // TODO(florian): make faster. (BTREE)
    for (ptrdiff_t i = 0; i < dom->boolPropsLen; i++) {
        if (dom->boolProps[i].nodeID == nodeID) {
            flo_html_indexID propID = dom->boolProps[i].propID;
            const flo_html_String boolProp =
                flo_html_getBoolProp(propID, dom, textStore);
            if ((status = flo_html_insertStringHashSet(set, boolProp)) !=
                HASH_SUCCESS) {
                FLO_HTML_ERROR_WITH_CODE_FORMAT(
                    flo_html_hashStatusToString(status),
                    "Failed to insert %.*s into hash set",
                    FLO_HTML_S_P(boolProp));
                flo_html_destroyStringHashSet(set);
                return status;
            }
        }
    }

    return status;
}

flo_html_ComparisonStatus compareBoolProps(const flo_html_Node *node1,
                                           const flo_html_Dom *dom1,
                                           const flo_html_TextStore *textStore1,
                                           const flo_html_Node *node2,
                                           const flo_html_Dom *dom2,
                                           const flo_html_TextStore *textStore2,
                                           const bool printDifferences) {
    flo_html_HashStatus hashStatus = HASH_SUCCESS;
    flo_html_StringHashSet set1;
    if ((hashStatus = createBoolPropsSet(node1->nodeID, dom1, textStore1,
                                         &set1)) != HASH_SUCCESS) {
        FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_hashStatusToString(hashStatus),
                                      "Failed to create hash set 1");
        return COMPARISON_MEMORY;
    }

    flo_html_StringHashSet set2;
    if ((hashStatus = createBoolPropsSet(node2->nodeID, dom2, textStore2,
                                         &set2)) != HASH_SUCCESS) {
        flo_html_destroyStringHashSet(&set1);
        FLO_HTML_ERROR_WITH_CODE_ONLY(flo_html_hashStatusToString(hashStatus),
                                      "Failed to create hash set 2");
        return COMPARISON_MEMORY;
    }

    flo_html_ComparisonStatus result =
        flo_html_equalsStringHashSet(&set1, &set2);

    if (printDifferences && result != COMPARISON_SUCCESS) {
        FLO_HTML_PRINT_ERROR("Nodes contain different bool props\n");
        printAttributes(node1->tagID, &set1, dom1, textStore1, node2->tagID,
                        &set2, dom2, textStore2);
    }

    flo_html_destroyStringHashSet(&set1);
    flo_html_destroyStringHashSet(&set2);

    return result;
}

bool tagStringEquals(const flo_html_TagRegistration *tagRegistration1,
                     const flo_html_TextStore *textStore1,
                     const flo_html_TagRegistration *tagRegistration2,
                     const flo_html_TextStore *textStore2) {
    const flo_html_String string1 = flo_html_getStringFromHashSet(
        &textStore1->tags.set, &tagRegistration1->hashElement);
    const flo_html_String string2 = flo_html_getStringFromHashSet(
        &textStore2->tags.set, &tagRegistration2->hashElement);
    return flo_html_stringEquals(string1, string2);
}

flo_html_ComparisonStatus
compareTags(const flo_html_Node *node1, const flo_html_Dom *dom1,
            const flo_html_TextStore *textStore1, const flo_html_Node *node2,
            const flo_html_Dom *dom2, const flo_html_TextStore *textStore2,
            const bool printDifferences) {
    if (node1->nodeType != node2->nodeType) {
        if (printDifferences) {
            const flo_html_String nodeType1 =
                flo_html_nodeTypeToString(node1->nodeType);
            const flo_html_String nodeType2 =
                flo_html_nodeTypeToString(node2->nodeType);
            FLO_HTML_PRINT_ERROR("Uncomparable nodes:\n"
                                 "node 1 type: %.*s\n"
                                 "node 2 type: %.*s\n",
                                 FLO_HTML_S_P(nodeType1),
                                 FLO_HTML_S_P(nodeType2));
        }
        return COMPARISON_DIFFERENT_NODE_TYPE;
    }

    switch (node1->nodeType) {
    case NODE_TYPE_TEXT: {
        const flo_html_String text1 = node1->text;
        const flo_html_String text2 = node2->text;

        if (flo_html_stringEquals(text1, text2)) {
            return COMPARISON_SUCCESS;
        }

        if (printDifferences) {
            FLO_HTML_PRINT_ERROR(
                "Different text nodes\nnode 1: %.*s\nnode 2: %.*s\n",
                FLO_HTML_S_P(text1), FLO_HTML_S_P(text2));
        }
        return COMPARISON_DIFFERENT_CONTENT;
    }
    case NODE_TYPE_DOCUMENT: {
        flo_html_TagRegistration *tagRegistration1 = NULL;
        flo_html_getTagRegistration(node1->tagID, dom1, &tagRegistration1);
        flo_html_TagRegistration *tagRegistration2 = NULL;
        flo_html_getTagRegistration(node2->tagID, dom2, &tagRegistration2);

        if (tagRegistration1->isPaired != tagRegistration2->isPaired) {
            if (printDifferences) {
                char singleNode = '1';
                if (!tagRegistration2->isPaired) {
                    singleNode++;
                }

                const flo_html_String tag1 = flo_html_getStringFromHashSet(
                    &textStore1->tags.set, &tagRegistration1->hashElement);
                const flo_html_String tag2 = flo_html_getStringFromHashSet(
                    &textStore2->tags.set, &tagRegistration2->hashElement);
                FLO_HTML_PRINT_ERROR(
                    "Uncomparable nodes: single node and paired node.\nFound "
                    "single node in node %c.\n"
                    "node 1 tag: %.*s\nnode 2 tag: %.*s\n",
                    singleNode, FLO_HTML_S_P(tag1), FLO_HTML_S_P(tag2));
            }
            return COMPARISON_DIFFERENT_NODE_TYPE;
        }

        if (!tagStringEquals(tagRegistration1, textStore1, tagRegistration2,
                             textStore2)) {
            if (printDifferences) {
                const flo_html_String tag1 =
                    flo_html_getTag(node1->nodeID, dom1, textStore1);
                const flo_html_String tag2 =
                    flo_html_getTag(node2->nodeID, dom2, textStore2);
                FLO_HTML_PRINT_ERROR(
                    "Nodes have different tags.\nnode 1 tag: %.*s\nnode "
                    "2 tag: %.*s\n",
                    FLO_HTML_S_P(tag1), FLO_HTML_S_P(tag2));
            }
            return COMPARISON_DIFFERENT_CONTENT;
        }

        return COMPARISON_SUCCESS;
    }
    default: {
        if (printDifferences) {
            const flo_html_String nodeType1 =
                flo_html_nodeTypeToString(node1->nodeType);
            FLO_HTML_PRINT_ERROR(
                "Comparison not implemented for this node type:"
                "node type: %.*s\n",
                FLO_HTML_S_P(nodeType1));
        }
        return COMPARISON_DIFFERENT_NODE_TYPE;
    }
    }
}

flo_html_ComparisonStatus
compareNode(flo_html_node_id *currNodeID1, const flo_html_Dom *dom1,
            const flo_html_TextStore *textStore1, flo_html_node_id *currNodeID2,
            const flo_html_Dom *dom2, const flo_html_TextStore *textStore2) {
    flo_html_Node node1 = dom1->nodes[*currNodeID1];
    flo_html_Node node2 = dom2->nodes[*currNodeID2];

    flo_html_ComparisonStatus result =
        compareTags(&node1, dom1, textStore1, &node2, dom2, textStore2, false);
    if (result != COMPARISON_SUCCESS) {
        return result;
    }

    if (node1.nodeType == NODE_TYPE_DOCUMENT) {
        result = compareBoolProps(&node1, dom1, textStore1, &node2, dom2,
                                  textStore2, false);
        if (result != COMPARISON_SUCCESS) {
            return result;
        }
        result = compareProps(&node1, dom1, textStore1, &node2, dom2,
                              textStore2, false);
        if (result != COMPARISON_SUCCESS) {
            return result;
        }
    }

    const flo_html_node_id parentNodeID1 = *currNodeID1;
    const flo_html_node_id parentNodeID2 = *currNodeID2;
    *currNodeID1 = flo_html_getFirstChild(*currNodeID1, dom1);
    *currNodeID2 = flo_html_getFirstChild(*currNodeID2, dom2);

    while (*currNodeID1 && *currNodeID2) {
        flo_html_ComparisonStatus comp = compareNode(
            currNodeID1, dom1, textStore1, currNodeID2, dom2, textStore2);
        if (comp != COMPARISON_SUCCESS) {
            return comp;
        }

        *currNodeID1 = flo_html_getNext(*currNodeID1, dom1);
        *currNodeID2 = flo_html_getNext(*currNodeID2, dom2);
    }

    if (*currNodeID1 ^ *currNodeID2) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    *currNodeID1 = parentNodeID1;
    *currNodeID2 = parentNodeID2;

    return COMPARISON_SUCCESS;
}

flo_html_ComparisonStatus
flo_html_equalsWithNode(flo_html_node_id *currNodeID1, const flo_html_Dom *dom1,
                        const flo_html_TextStore *textStore1,
                        flo_html_node_id *currNodeID2, const flo_html_Dom *dom2,
                        const flo_html_TextStore *textStore2) {
    *currNodeID1 = dom1->firstNodeID;
    *currNodeID2 = dom2->firstNodeID;
    while (*currNodeID1 && *currNodeID2) {
        flo_html_ComparisonStatus comp = compareNode(
            currNodeID1, dom1, textStore1, currNodeID2, dom2, textStore2);
        if (comp != COMPARISON_SUCCESS) {
            return comp;
        }

        *currNodeID1 = flo_html_getNext(*currNodeID1, dom1);
        *currNodeID2 = flo_html_getNext(*currNodeID2, dom2);
    }

    if (*currNodeID1 ^ *currNodeID2) {
        return COMPARISON_DIFFERENT_SIZES;
    }

    return COMPARISON_SUCCESS;
}

flo_html_ComparisonStatus
flo_html_equals(const flo_html_Dom *dom1, const flo_html_TextStore *textStore1,
                const flo_html_Dom *dom2,
                const flo_html_TextStore *textStore2) {
    flo_html_node_id nodeID1 = 0;
    flo_html_node_id nodeID2 = 0;
    return flo_html_equalsWithNode(&nodeID1, dom1, textStore1, &nodeID2, dom2,
                                   textStore2);
}

void flo_html_printFirstDifference(const flo_html_node_id nodeID1,
                                   const flo_html_Dom *dom1,
                                   const flo_html_TextStore *textStore1,
                                   const flo_html_node_id nodeID2,
                                   const flo_html_Dom *dom2,
                                   const flo_html_TextStore *textStore2) {
    flo_html_Node *node1 = &dom1->nodes[nodeID1];
    flo_html_Node *node2 = &dom2->nodes[nodeID2];

    if (compareTags(node1, dom1, textStore1, node2, dom2, textStore2, true) !=
        COMPARISON_SUCCESS) {
        return;
    }

    if (node1->nodeType == NODE_TYPE_DOCUMENT) {
        if (compareBoolProps(node1, dom1, textStore1, node2, dom2, textStore2,
                             true) != COMPARISON_SUCCESS) {
            return;
        }
        if (compareProps(node1, dom1, textStore1, node2, dom2, textStore2,
                         true) != COMPARISON_SUCCESS) {
            return;
        }
    }

    FLO_HTML_PRINT_ERROR(
        "Could not find any differences for the supplied nodes & documents "
        "combination.\n");
}
