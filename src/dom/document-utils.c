
#include "flo/html-parser/dom/document-utils.h"

const char *getText(const node_id nodeID, const Document *doc,
                    const DataContainer *dataContainer) {
    for (size_t i = 0; i < doc->textLen; i++) {
        TextNode textNode = doc->text[i];

        if (textNode.nodeID == nodeID) {
            char *text =
                dataContainer->text.container.elements[textNode.textID];
            return text;
        }
    }

    return NULL;
}
