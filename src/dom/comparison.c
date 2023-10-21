#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "flo/html-parser/comparison-status.h"
#include "flo/html-parser/dom/comparison.h"
#include "flo/html-parser/dom/dom-util.h"
#include "flo/html-parser/dom/traversal.h"
#include "flo/html-parser/parser.h"
#include "flo/html-parser/type/element/elements-print.h"
#include "flo/html-parser/util/error.h"

flo_html_ComparisonStatus convertHashComparisonToComparison(
    const flo_html_HashComparisonStatus hashComparisonStatus) {
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

void printAttributes(const flo_html_index_id tagID1,
                     const flo_html_StringHashSet *set1,
                     flo_html_ParsedHTML parsed1,
                     const flo_html_index_id tagID2,
                     const flo_html_StringHashSet *set2,
                     flo_html_ParsedHTML parsed2) {
    const flo_html_String tag1 = flo_html_getTag(tagID1, parsed1);
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

    const flo_html_String tag2 = flo_html_getTag(tagID2, parsed2);
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

bool createPropsSet(const flo_html_node_id nodeID, flo_html_ParsedHTML parsed,
                    flo_html_StringHashSet *keySet,
                    flo_html_StringHashSet *valueSet, flo_html_Arena *perm) {
    flo_html_Dom *dom = parsed.dom;

    *keySet = flo_html_initStringHashSet(FLO_HTML_MAX_PROPERTIES * 2, perm);
    *valueSet = flo_html_initStringHashSet(FLO_HTML_MAX_PROPERTIES * 2, perm);

    // TODO(florian): make faster. (BTREE)
    for (ptrdiff_t i = 0; i < dom->props.len; i++) {
        if (dom->props.buf[i].nodeID == nodeID) {
            flo_html_index_id keyID = dom->props.buf[i].keyID;
            const flo_html_String propKey = flo_html_getPropKey(keyID, parsed);
            if (!flo_html_insertStringHashSet(keySet, propKey, perm)) {
                FLO_HTML_PRINT_ERROR(
                    "Failed to insert \"%.*s\" into key hash set\n",
                    FLO_HTML_S_P(propKey));
                return false;
            }

            flo_html_index_id valueID = dom->props.buf[i].valueID;
            const flo_html_String propValue =
                flo_html_getPropValue(valueID, parsed);
            if (!flo_html_insertStringHashSet(keySet, propValue, perm)) {
                FLO_HTML_PRINT_ERROR(
                    "Failed to insert \"%.*s\" into value hash set\n",
                    FLO_HTML_S_P(propValue));
                return false;
            }
        }
    }

    return true;
}

// TODO: this function does not correct compare properties
// e.g. <a thing="a", foo="bar" /> IS NOT EQUAL TO
//      <a foo="a", thing="bar" />
flo_html_ComparisonStatus
compareProps(const flo_html_Node node1, flo_html_ParsedHTML parsed1,
             const flo_html_Node node2, flo_html_ParsedHTML parsed2,
             const bool printDifferences, flo_html_Arena scratch) {
    flo_html_StringHashSet keySet1;
    flo_html_StringHashSet valueSet1;
    if (!createPropsSet(node1.nodeID, parsed1, &keySet1, &valueSet1,
                        &scratch)) {
        FLO_HTML_PRINT_ERROR("Failed to create hash sets for node 1\n");
        return COMPARISON_MEMORY;
    }

    flo_html_StringHashSet keySet2;
    flo_html_StringHashSet valueSet2;
    if (!createPropsSet(node2.nodeID, parsed2, &keySet2, &valueSet2,
                        &scratch)) {
        FLO_HTML_PRINT_ERROR("Failed to create hash sets for node 2\n");
        return COMPARISON_MEMORY;
    }

    flo_html_HashComparisonStatus result =
        flo_html_equalsStringHashSet(&keySet1, &keySet2);
    if (result != HASH_COMPARISON_SUCCESS) {
        if (printDifferences) {
            FLO_HTML_ERROR_WITH_CODE_ONLY(
                flo_html_hashComparisonStatusToString(result),
                "Nodes contain different key props\n");
            printAttributes(node1.tagID, &keySet1, parsed1, node2.tagID,
                            &keySet2, parsed2);
        }
        return convertHashComparisonToComparison(result);
    }

    result = flo_html_equalsStringHashSet(&valueSet1, &valueSet2);
    if (result != HASH_COMPARISON_SUCCESS) {
        if (printDifferences) {
            FLO_HTML_ERROR_WITH_CODE_ONLY(
                flo_html_hashComparisonStatusToString(result),
                "Nodes contain different value props\n");
            printAttributes(node1.tagID, &valueSet1, parsed1, node2.tagID,
                            &valueSet2, parsed2);
        }
        return convertHashComparisonToComparison(result);
    }

    return COMPARISON_SUCCESS;
}

bool createBoolPropsSet(const flo_html_node_id nodeID,
                        flo_html_ParsedHTML parsed,
                        flo_html_StringHashSet *boolPropsSet,
                        flo_html_Arena *perm) {
    *boolPropsSet =
        flo_html_initStringHashSet(FLO_HTML_MAX_PROPERTIES * 2, perm);

    flo_html_Dom *dom = parsed.dom;

    // TODO(florian): make faster. (BTREE)
    for (ptrdiff_t i = 0; i < dom->boolProps.len; i++) {
        if (dom->boolProps.buf[i].nodeID == nodeID) {
            flo_html_index_id propID = dom->boolProps.buf[i].propID;
            const flo_html_String boolProp =
                flo_html_getBoolProp(propID, parsed);
            if (!flo_html_insertStringHashSet(boolPropsSet, boolProp, perm)) {
                FLO_HTML_PRINT_ERROR("Failed to insert %.*s into hash set",
                                     FLO_HTML_S_P(boolProp));
                return false;
            }
        }
    }

    return true;
}

flo_html_ComparisonStatus
compareBoolProps(const flo_html_Node node1, flo_html_ParsedHTML parsed1,
                 const flo_html_Node node2, flo_html_ParsedHTML parsed2,
                 const bool printDifferences, flo_html_Arena scratch) {
    flo_html_StringHashSet set1;
    if (!(createBoolPropsSet(node1.nodeID, parsed1, &set1, &scratch))) {
        FLO_HTML_PRINT_ERROR("Failed to create hash set 1");
        return COMPARISON_MEMORY;
    }

    flo_html_StringHashSet set2;
    if (!(createBoolPropsSet(node2.nodeID, parsed2, &set2, &scratch))) {
        FLO_HTML_PRINT_ERROR("Failed to create hash set 2");
        return COMPARISON_MEMORY;
    }

    flo_html_HashComparisonStatus result =
        flo_html_equalsStringHashSet(&set1, &set2);
    if (result != HASH_COMPARISON_SUCCESS) {
        if (printDifferences) {
            FLO_HTML_PRINT_ERROR("Nodes contain different bool props\n");
            printAttributes(node1.tagID, &set1, parsed1, node2.tagID, &set2,
                            parsed2);
        }
        return convertHashComparisonToComparison(result);
    }

    return COMPARISON_SUCCESS;
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

flo_html_ComparisonStatus compareTags(const flo_html_Node node1,
                                      flo_html_ParsedHTML parsed1,
                                      const flo_html_Node node2,
                                      flo_html_ParsedHTML parsed2,
                                      const bool printDifferences) {
    if (node1.nodeType != node2.nodeType) {
        if (printDifferences) {
            const flo_html_String nodeType1 =
                flo_html_nodeTypeToString(node1.nodeType);
            const flo_html_String nodeType2 =
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
        const flo_html_String text1 = node1.text;
        const flo_html_String text2 = node2.text;

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
            &parsed1.dom->tagRegistry.buf[node1.tagID];
        flo_html_TagRegistration *tagRegistration2 =
            &parsed2.dom->tagRegistry.buf[node2.tagID];

        if (tagRegistration1->isPaired != tagRegistration2->isPaired) {
            if (printDifferences) {
                char singleNode = '1';
                if (!tagRegistration2->isPaired) {
                    singleNode++;
                }

                const flo_html_String tag1 = flo_html_getStringFromHashSet(
                    &parsed1.textStore->tags.set,
                    &tagRegistration1->hashElement);
                const flo_html_String tag2 = flo_html_getStringFromHashSet(
                    &parsed2.textStore->tags.set,
                    &tagRegistration2->hashElement);
                FLO_HTML_PRINT_ERROR(
                    "Uncomparable nodes: single node and paired node.\nFound "
                    "single node in node %c.\n"
                    "node 1 tag: %.*s\nnode 2 tag: %.*s\n",
                    singleNode, FLO_HTML_S_P(tag1), FLO_HTML_S_P(tag2));
            }
            return COMPARISON_DIFFERENT_NODE_TYPE;
        }

        if (!tagStringEquals(tagRegistration1, parsed1.textStore,
                             tagRegistration2, parsed2.textStore)) {
            if (printDifferences) {
                const flo_html_String tag1 =
                    flo_html_getTag(node1.nodeID, parsed1);
                const flo_html_String tag2 =
                    flo_html_getTag(node2.nodeID, parsed2);
                FLO_HTML_PRINT_ERROR(
                    "Nodes have different tags.\nnode 1 tag: %.*s\nnode "
                    "2 tag: %.*s\n",
                    FLO_HTML_S_P(tag1), FLO_HTML_S_P(tag2));
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
            const flo_html_String nodeType1 =
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
                                      flo_html_ParsedHTML parsed1,
                                      flo_html_node_id nodeID2,
                                      flo_html_ParsedHTML parsed2,
                                      flo_html_Arena scratch) {
    flo_html_ComparisonResult result;
    result.nodeID1 = nodeID1;
    result.nodeID2 = nodeID2;

    flo_html_Node node1 = parsed1.dom->nodes.buf[nodeID1];
    flo_html_Node node2 = parsed2.dom->nodes.buf[nodeID2];

    result.status = compareTags(node1, parsed1, node2, parsed2, false);
    if (result.status != COMPARISON_SUCCESS) {
        return result;
    }

    if (node1.nodeType == NODE_TYPE_DOCUMENT) {
        result.status =
            compareBoolProps(node1, parsed1, node2, parsed2, false, scratch);
        if (result.status != COMPARISON_SUCCESS) {
            return result;
        }

        result.status =
            compareProps(node1, parsed1, node2, parsed2, false, scratch);
        if (result.status != COMPARISON_SUCCESS) {
            return result;
        }
    }

    flo_html_node_id childNode1 = flo_html_getFirstChild(nodeID1, parsed1.dom);
    flo_html_node_id childNode2 = flo_html_getFirstChild(nodeID2, parsed2.dom);

    while (childNode1 && childNode2) {
        flo_html_ComparisonResult childResult =
            compareNode(childNode1, parsed1, childNode2, parsed2, scratch);
        if (childResult.status != COMPARISON_SUCCESS) {
            return childResult;
        }

        childNode1 = flo_html_getNext(childNode1, parsed1.dom);
        childNode2 = flo_html_getNext(childNode2, parsed2.dom);
    }

    if (childNode1 ^ childNode2) {
        result.status = COMPARISON_DIFFERENT_SIZES;
        return result;
    }

    result.status = COMPARISON_SUCCESS;
    return result;
}

flo_html_ComparisonResult flo_html_equals(flo_html_ParsedHTML parsed1,
                                          flo_html_ParsedHTML parsed2,
                                          flo_html_Arena scratch) {
    flo_html_ComparisonResult result;

    flo_html_node_id currNodeID1 = FLO_HTML_ROOT_NODE_ID;
    flo_html_node_id currNodeID2 = FLO_HTML_ROOT_NODE_ID;
    while (currNodeID1 && currNodeID2) {
        result =
            compareNode(currNodeID1, parsed1, currNodeID2, parsed2, scratch);
        if (result.status != COMPARISON_SUCCESS) {
            return result;
        }

        currNodeID1 = flo_html_getNext(currNodeID1, parsed1.dom);
        currNodeID2 = flo_html_getNext(currNodeID2, parsed2.dom);
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

void flo_html_printFirstDifference(const flo_html_node_id nodeID1,
                                   flo_html_ParsedHTML parsed1,
                                   const flo_html_node_id nodeID2,
                                   flo_html_ParsedHTML parsed2,
                                   flo_html_Arena scratch) {
    flo_html_Node node1 = parsed1.dom->nodes.buf[nodeID1];
    flo_html_Node node2 = parsed2.dom->nodes.buf[nodeID2];

    if (compareTags(node1, parsed1, node2, parsed2, true) !=
        COMPARISON_SUCCESS) {
        return;
    }

    if (node1.nodeType == NODE_TYPE_DOCUMENT) {
        if (compareBoolProps(node1, parsed1, node2, parsed2, true, scratch) !=
            COMPARISON_SUCCESS) {
            return;
        }
        if (compareProps(node1, parsed1, node2, parsed2, true, scratch) !=
            COMPARISON_SUCCESS) {
            return;
        }
    }

    FLO_HTML_PRINT_ERROR(
        "Could not find any differences for the supplied nodes & documents "
        "combination.\n");
}
