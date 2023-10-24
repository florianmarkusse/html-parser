#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "flo/html-parser/comparison-status.h"
#include "flo/html-parser/dom/comparison.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/util/error.h"

flo_html_ComparisonStatus convertHashComparisonToComparison(
    flo_html_HashComparisonStatus hashComparisonStatus) {
    switch (hashComparisonStatus) {
    case HASH_COMPARISON_DIFFERENT_SIZES: {
        return COMPARISON_DIFFERENT_SIZES;
    }
    case HASH_COMPARISON_DIFFERENT_CONTENT: {
        return COMPARISON_DIFFERENT_CONTENT;
    }
    default: {
        FLO_HTML_ERROR_WITH_CODE_ONLY(
            flo_html_hashComparisonStatusToString(hashComparisonStatus),
            "unhandled comparison hashComparisonStatus\n");
        return COMPARISON_MEMORY;
    }
    }
}

void printAttributes(flo_html_index_id tagID1, flo_html_StringHashSet *set1,
                     flo_html_Dom *dom1, flo_html_index_id tagID2,
                     flo_html_StringHashSet *set2, flo_html_Dom *dom2) {
    FLO_HTML_PRINT_ERROR(
        "Printing certain attributes of node 1 with tag %.*s:\n",
        FLO_HTML_S_P(dom1->tagRegistry.buf[tagID1].tag));

    flo_html_StringHashSetIterator iterator =
        (flo_html_StringHashSetIterator){.set = set1, .index = 0};
    flo_html_String attribute;
    while ((attribute = flo_html_nextStringHashSetIterator(&iterator)).len !=
           0) {
        FLO_HTML_PRINT_ERROR("%.*s\n", FLO_HTML_S_P(attribute));
    }

    FLO_HTML_PRINT_ERROR(
        "Printing certain attributes of node 2 with tag %.*s:\n",
        FLO_HTML_S_P(dom2->tagRegistry.buf[tagID2].tag));

    iterator = (flo_html_StringHashSetIterator){.set = set2, .index = 0};
    while ((attribute = flo_html_nextStringHashSetIterator(&iterator)).len !=
           0) {
        FLO_HTML_PRINT_ERROR("%.*s\n", FLO_HTML_S_P(attribute));
    }
}

void createPropsSet(flo_html_node_id nodeID, flo_html_Dom *dom,
                    flo_html_StringHashSet *keySet,
                    flo_html_StringHashSet *valueSet, flo_html_Arena *perm) {
    *keySet = flo_html_initStringHashSet(FLO_HTML_MAX_PROPERTIES * 2, perm);
    *valueSet = flo_html_initStringHashSet(FLO_HTML_MAX_PROPERTIES * 2, perm);

    // TODO(florian): make faster. (BTREE)
    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        if (dom->props.buf[i].nodeID == nodeID) {
            flo_html_index_id keyID = dom->props.buf[i].keyID;
            flo_html_insertStringHashSet(keySet,
                                         dom->propKeyRegistry.buf[keyID], perm);

            flo_html_index_id valueID = dom->props.buf[i].valueID;
            flo_html_insertStringHashSet(
                keySet, dom->propValueRegistry.buf[valueID], perm);
        }
    }
}

// TODO: this function does not correct compare properties
// e.g. <a thing="a", foo="bar" /> IS NOT EQUAL TO
//      <a foo="a", thing="bar" />
flo_html_ComparisonStatus compareProps(flo_html_Node node1, flo_html_Dom *dom1,
                                       flo_html_Node node2, flo_html_Dom *dom2,
                                       bool printDifferences,
                                       flo_html_Arena scratch) {
    flo_html_StringHashSet keySet1;
    flo_html_StringHashSet valueSet1;
    createPropsSet(node1.nodeID, dom1, &keySet1, &valueSet1, &scratch);

    flo_html_StringHashSet keySet2;
    flo_html_StringHashSet valueSet2;
    createPropsSet(node2.nodeID, dom2, &keySet2, &valueSet2, &scratch);

    flo_html_HashComparisonStatus result =
        flo_html_equalsStringHashSet(&keySet1, &keySet2);
    if (result != HASH_COMPARISON_SUCCESS) {
        if (printDifferences) {
            FLO_HTML_ERROR_WITH_CODE_ONLY(
                flo_html_hashComparisonStatusToString(result),
                "Nodes contain different key props\n");
            printAttributes(node1.tagID, &keySet1, dom1, node2.tagID, &keySet2,
                            dom2);
        }
        return convertHashComparisonToComparison(result);
    }

    result = flo_html_equalsStringHashSet(&valueSet1, &valueSet2);
    if (result != HASH_COMPARISON_SUCCESS) {
        if (printDifferences) {
            FLO_HTML_ERROR_WITH_CODE_ONLY(
                flo_html_hashComparisonStatusToString(result),
                "Nodes contain different value props\n");
            printAttributes(node1.tagID, &valueSet1, dom1, node2.tagID,
                            &valueSet2, dom2);
        }
        return convertHashComparisonToComparison(result);
    }

    return COMPARISON_SUCCESS;
}

bool createBoolPropsSet(flo_html_node_id nodeID, flo_html_Dom *dom,
                        flo_html_StringHashSet *boolPropsSet,
                        flo_html_Arena *perm) {
    *boolPropsSet =
        flo_html_initStringHashSet(FLO_HTML_MAX_PROPERTIES * 2, perm);

    // TODO(florian): make faster. (BTREE)
    for (ptrdiff_t i = 0; i < dom->boolProps.len; i++) {
        if (dom->boolProps.buf[i].nodeID == nodeID) {
            flo_html_index_id propID = dom->boolProps.buf[i].propID;
            flo_html_insertStringHashSet(
                boolPropsSet, dom->boolPropRegistry.buf[propID], perm);
        }
    }

    return true;
}

flo_html_ComparisonStatus
compareBoolProps(flo_html_Node node1, flo_html_Dom *dom1, flo_html_Node node2,
                 flo_html_Dom *dom2, bool printDifferences,
                 flo_html_Arena scratch) {
    flo_html_StringHashSet set1;
    if (!(createBoolPropsSet(node1.nodeID, dom1, &set1, &scratch))) {
        FLO_HTML_PRINT_ERROR("Failed to create hash set 1");
        return COMPARISON_MEMORY;
    }

    flo_html_StringHashSet set2;
    if (!(createBoolPropsSet(node2.nodeID, dom2, &set2, &scratch))) {
        FLO_HTML_PRINT_ERROR("Failed to create hash set 2");
        return COMPARISON_MEMORY;
    }

    flo_html_HashComparisonStatus result =
        flo_html_equalsStringHashSet(&set1, &set2);
    if (result != HASH_COMPARISON_SUCCESS) {
        if (printDifferences) {
            FLO_HTML_PRINT_ERROR("Nodes contain different bool props\n");
            printAttributes(node1.tagID, &set1, dom1, node2.tagID, &set2, dom2);
        }
        return convertHashComparisonToComparison(result);
    }

    return COMPARISON_SUCCESS;
}

flo_html_ComparisonStatus compareTags(flo_html_Node node1, flo_html_Dom *dom1,
                                      flo_html_Node node2, flo_html_Dom *dom2,
                                      bool printDifferences) {
    if (node1.nodeType != node2.nodeType) {
        if (printDifferences) {
            flo_html_String nodeType1 =
                flo_html_nodeTypeToString(node1.nodeType);
            flo_html_String nodeType2 =
                flo_html_nodeTypeToString(node2.nodeType);
            FLO_HTML_PRINT_ERROR("Uncomparable nodes:\n"
                                 "node 1 type: %.*s\n"
                                 "node 2 type: %.*s\n",
                                 FLO_HTML_S_P(nodeType1),
                                 FLO_HTML_S_P(nodeType2));
        }
        return COMPARISON_DIFFERENT_NODE_TYPE;
    }

    switch (node1.nodeType) {
    case NODE_TYPE_TEXT: {
        flo_html_String text1 = node1.text;
        flo_html_String text2 = node2.text;

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
        flo_html_TagRegistration *tagRegistration1 =
            &dom1->tagRegistry.buf[node1.tagID];
        flo_html_TagRegistration *tagRegistration2 =
            &dom2->tagRegistry.buf[node2.tagID];

        if (tagRegistration1->isPaired != tagRegistration2->isPaired) {
            if (printDifferences) {
                char singleNode = '1';
                if (!tagRegistration2->isPaired) {
                    singleNode++;
                }

                FLO_HTML_PRINT_ERROR(
                    "Uncomparable nodes: single node and paired node.\nFound "
                    "single node in node %c.\n"
                    "node 1 tag: %.*s\nnode 2 tag: %.*s\n",
                    singleNode, FLO_HTML_S_P(tagRegistration1->tag),
                    FLO_HTML_S_P(tagRegistration2->tag));
            }
            return COMPARISON_DIFFERENT_NODE_TYPE;
        }

        if (!flo_html_stringEquals(tagRegistration1->tag,
                                   tagRegistration2->tag)) {
            if (printDifferences) {
                FLO_HTML_PRINT_ERROR(
                    "Nodes have different tags.\nnode 1 tag: %.*s\nnode "
                    "2 tag: %.*s\n",
                    FLO_HTML_S_P(tagRegistration1->tag),
                    FLO_HTML_S_P(tagRegistration2->tag));
            }
            return COMPARISON_DIFFERENT_CONTENT;
        }

        return COMPARISON_SUCCESS;
    }
    case NODE_TYPE_ROOT: {
        return COMPARISON_SUCCESS;
    }
    default: {
        if (printDifferences) {
            flo_html_String nodeType1 =
                flo_html_nodeTypeToString(node1.nodeType);
            FLO_HTML_PRINT_ERROR(
                "Comparison not implemented for this node type:"
                "node type: %.*s\n",
                FLO_HTML_S_P(nodeType1));
        }
        return COMPARISON_DIFFERENT_NODE_TYPE;
    }
    }
}

flo_html_ComparisonResult compareNode(flo_html_node_id nodeID1,
                                      flo_html_Dom *dom1,
                                      flo_html_node_id nodeID2,
                                      flo_html_Dom *dom2,
                                      flo_html_Arena scratch) {
    flo_html_ComparisonResult result;
    result.nodeID1 = nodeID1;
    result.nodeID2 = nodeID2;

    flo_html_Node node1 = dom1->nodes.buf[nodeID1];
    flo_html_Node node2 = dom2->nodes.buf[nodeID2];

    result.status = compareTags(node1, dom1, node2, dom2, false);
    if (result.status != COMPARISON_SUCCESS) {
        return result;
    }

    if (node1.nodeType == NODE_TYPE_DOCUMENT) {
        result.status =
            compareBoolProps(node1, dom1, node2, dom2, false, scratch);
        if (result.status != COMPARISON_SUCCESS) {
            return result;
        }

        result.status = compareProps(node1, dom1, node2, dom2, false, scratch);
        if (result.status != COMPARISON_SUCCESS) {
            return result;
        }
    }

    flo_html_node_id childNode1 = flo_html_getFirstChild(nodeID1, dom1);
    flo_html_node_id childNode2 = flo_html_getFirstChild(nodeID2, dom2);

    while (childNode1 && childNode2) {
        flo_html_ComparisonResult childResult =
            compareNode(childNode1, dom1, childNode2, dom2, scratch);
        if (childResult.status != COMPARISON_SUCCESS) {
            return childResult;
        }

        childNode1 = flo_html_getNext(childNode1, dom1);
        childNode2 = flo_html_getNext(childNode2, dom2);
    }

    if (childNode1 ^ childNode2) {
        result.status = COMPARISON_DIFFERENT_SIZES;
        return result;
    }

    result.status = COMPARISON_SUCCESS;
    return result;
}

flo_html_ComparisonResult flo_html_equals(flo_html_Dom *dom1,
                                          flo_html_Dom *dom2,
                                          flo_html_Arena scratch) {
    flo_html_ComparisonResult result;

    flo_html_node_id currNodeID1 = FLO_HTML_ROOT_NODE_ID;
    flo_html_node_id currNodeID2 = FLO_HTML_ROOT_NODE_ID;
    while (currNodeID1 && currNodeID2) {
        result = compareNode(currNodeID1, dom1, currNodeID2, dom2, scratch);
        if (result.status != COMPARISON_SUCCESS) {
            return result;
        }

        currNodeID1 = flo_html_getNext(currNodeID1, dom1);
        currNodeID2 = flo_html_getNext(currNodeID2, dom2);
    }

    if (currNodeID1 ^ currNodeID2) {
        result.status = COMPARISON_DIFFERENT_SIZES;
        result.nodeID1 = currNodeID1;
        result.nodeID2 = currNodeID2;
        return result;
    }

    result.status = COMPARISON_SUCCESS;
    return result;
}

void flo_html_printFirstDifference(flo_html_node_id nodeID1, flo_html_Dom *dom1,
                                   flo_html_node_id nodeID2, flo_html_Dom *dom2,
                                   flo_html_Arena scratch) {
    flo_html_Node node1 = dom1->nodes.buf[nodeID1];
    flo_html_Node node2 = dom2->nodes.buf[nodeID2];

    if (compareTags(node1, dom1, node2, dom2, true) != COMPARISON_SUCCESS) {
        return;
    }

    if (node1.nodeType == NODE_TYPE_DOCUMENT) {
        if (compareBoolProps(node1, dom1, node2, dom2, true, scratch) !=
            COMPARISON_SUCCESS) {
            return;
        }
        if (compareProps(node1, dom1, node2, dom2, true, scratch) !=
            COMPARISON_SUCCESS) {
            return;
        }
    }

    FLO_HTML_PRINT_ERROR(
        "Could not find any differences for the supplied nodes & documents "
        "combination.\n");
}
