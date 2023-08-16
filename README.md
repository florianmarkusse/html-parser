# HTML-PARSER

This project contains an (in-progress) implementation of a lenient HTML-parser.

**Currently, this project is only good for minfying HTML and querying the document. Modifications are not yet supported. Please come back later or [contribute](#contributing) to speed up the process of this project.**

## Functionalities
- Reading an HTML file or string into a `Document`.
- Querying over the document with CSS queries.
- Writing out a `Document` to a file in minified HTML.

## Feedback & Assistance
If you encounter any challenges or have suggestions related to the functionalities provided by this library, please do not hesitate to:
- [Open an issue](https://github.com/florianmarkusse/html-parser/issues/new/choose), or;
- [Open a PR](https://github.com/florianmarkusse/html-parser/compare).

We value your input and are committed to improving the project based on your feedback. Moreover, I would be absolutely delighted to see someone using my library :).

## Planned functionalities
- Modifying the `Document`.
- Cross-platform installation.

## Compiling
The shell scripts used to install dependencies and build the project are written for linux only. The program itself is cross-platform, however.
1. Clone the repository to your favorite folder.
2. Run ```./install-dependencies.sh``` to install all dependencies (*linux* only currently).
2. Run ```./build-run.sh html-parser``` to build the library and run the tests (*linux* only currently).


## Under the Hood

This section describes how the parser works under the hood to give the reader a better overview.

### HTML parsing

The parsing of a possible HTML string is done by a state machine that loops over the tokens one by one. Unlike a strict parser, this parsing is done in a lenient manner. I.e., the parser does not care whether or not you comply to the strict [HTML specification](https://html.spec.whatwg.org/), and instead tries its best to make the most out of what was provided.

#### Types

The parser differentiates between 2 node types:
- *Document node*, this node is further divided into 2 subtypes:
    - *Single node*, for example: ```<input />```.
    - *Paired node*, for example: ```<div></div>```.
- *Text node*, for example: ```I AM THE TEXT NODE```. The text node simply contains text, and is often found inside paired nodes, for example: ```<p>example</p>```. 

#### Document

The HTML string is parsed into a `Document`. Unlike a tree of nodes, the layout of the `Document` follows a data-oriented pattern. The following tables make up the `Document`:

---
##### node
```nodeID``` | ```tagID``` 
---|---
---

##### parent-first-child
```parentID``` | ```childID```
---|---
---

##### next-node
```currentNodeID``` | ```nextNodeID```  
---|---
---

##### boolean-property
```nodeID``` | ```propID```  
---|---
---

##### property
```nodeID``` | ```keyID``` | ```valueID```  
---|---|---
---

##### text-node
```nodeID``` | ```textID``` 
---|---
---

As you can see, there is no actual text stored inside the `Document` and merely refers to IDs. Each ID is a reference to arrays that hold the actual text and can be found by indexing this array. 

For example, imagine we have an entry in the *node* table, ```{ 4, 5 }```, and we want to find the text representation of this text. This can be done by ```dataContainer.tags.container.elements[5] -> "div"'```.

All data the IDs are pointing to are stored inside a `DataContainer`:
```C
typedef struct {
    CombinedElements tags; // Holds all the values of the tags.
    CombinedElements propKeys; // Holds all the keys of properties. A boolean property is treated as a property with only a key.
    Elements propValues; // Holds all the values of properties.
    Elements text; // Holds all the values of the text nodes.
} DataContainer;
```

Why was this decision made instead of storing all this data inside the `Document` itself?
- It allows sharing of tags when parsing multiple documents, e.g., a ```<p>``` in multiple documents has the same ```tagID```.
- It allows for faster filtering of nodes.
- [the maintainer](https://github.com/florianmarkusse) wanted to experiment with data-oriented programming and this seemed like a fun way to practise it.

### Querying
Querying the `Document` is possible or planned through all convenience methods also present in querying a DOM on the web, such as `querySelectorAll`, `getElementsByTagName`, and `getElementByID`. Similar to the HTML parsing, the parsing is done in the most lenient way possible using a state machine that loops over the css query.

## Contributing
It would be amazing if you are willing to contribute to the project. Please look at any issues if they are present or reach out to [the maintainer](https://github.com/florianmarkusse) to collaborate!

---
**NB**: Since this parser is lenient, it can probably also be used to parse XML, or similar markup languages. Be advised, this has not been tested and is not the goal of this project.

