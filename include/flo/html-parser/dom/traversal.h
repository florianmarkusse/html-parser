#ifndef FLO_HTML_PARSER_DOM_TRAVERSAL_H
#define FLO_HTML_PARSER_DOM_TRAVERSAL_H

#include "dom.h"

node_id getFirstChild(node_id currentNodeID, const Dom *dom);
ParentChild *getFirstChildNode(node_id currentNodeID, const Dom *dom);
node_id getNext(node_id currentNodeID, const Dom *dom);
NextNode *getNextNode(node_id currentNodeID, const Dom *dom);

node_id getPrevious(node_id currentNodeID, const Dom *dom);
NextNode *getPreviousNode(node_id currentNodeID, const Dom *dom);

node_id getParent(node_id currentNodeID, const Dom *dom);
ParentChild *getParentNode(node_id currentNodeID, const Dom *dom);

node_id traverseDom(node_id currentNodeID, const Dom *dom);
node_id traverseNode(node_id currentNodeID, node_id toTraverseNodeID,
                     const Dom *dom);
node_id getLastNext(node_id startNodeID, const Dom *dom);

#endif
