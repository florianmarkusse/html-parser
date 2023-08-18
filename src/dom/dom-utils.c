
#include "flo/html-parser/dom/dom-utils.h"

const char *getText(const node_id nodeID, const Dom *dom,
                    const DataContainer *dataContainer) {
    for (size_t i = 0; i < dom->textLen; i++) {
        TextNode textNode = dom->text[i];

        if (textNode.nodeID == nodeID) {
            char *text =
                dataContainer->text.container.elements[textNode.textID];
            return text;
        }
    }

    return NULL;
}
