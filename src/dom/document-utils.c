
#include "dom/document-utils.h"

const char *getText(const node_id nodeID, const Document *doc) {
    for (size_t i = 0; i < doc->textLen; i++) {
        TextNode textNode = doc->text[i];

        if (textNode.nodeID == nodeID) {
            char *text = gText.container.elements[textNode.textID];
            return text;
        }
    }

    return NULL;
}
