# HTML-PARSER: Unleash the Power of Seamless HTML Parsing!

Are you ready to dive into the world of effortless HTML parsing? Look no further! 🚀

`html-parser` is not your ordinary HTML parsing library; it's a game-changer that combines speed, simplicity, and versatility in one extraordinary package. Say goodbye to the days of wrestling with clunky parsing tools and complex setups. With HTML-PARSER, you're in control!

## What Makes `html-parser` So Special?

- **No External Dependencies**: We believe in keeping things simple. That's why `html-parser` has zero external dependencies. Just grab it, build it, and you're good to go!

- **Limitless Manipulation**: With a plethora of functions at your disposal, you can manipulate HTML documents like never before. Extract data, modify elements, or traverse the DOM tree with ease.

- **Robust & Reliable**: `html-parser` is built with robustness in mind. It can gracefully handle even the most lenient HTML documents, so you can focus on your project without worrying about parsing quirks.

- **Fast**: `html-parser` is designed for speed. Find benchmark results on [html-parser-benchmarks](https://github.com/florianmarkusse/html-parser-benchmarks)!

## Platform Flexibility

`html-parser` currently operates on Unix systems only due to its utilization of specific memory allocation headers. If you're eager to extend its compatibility to Windows or other platforms, we welcome your contributions! Feel free to open a pull request :).

## Table of Contents

- [Demo Projects](#demo-projects)
- [Quick Example](#quick-example)
- [Functionalities](#functionalities)
- [Extending and Building Upon](#extending-and-building-upon)
- [Feedback & Assistance](#feedback--assistance)
- [Under the Hood](#under-the-hood)
- [Querying](#querying)
- [Traversal](#traversal)
- [Modifying](#modifying)
- [Printing & Writing](#printing-%26-writing)
- [Contributing](#contributing)
- [License](#license)


## Demo Projects

To see complete examples of how to install and use the `html-parser` library in different projects, you can explore the following repositories:

1. [C Demo Project](https://github.com/florianmarkusse/html-parser-demo-c): This demo project provides a practical demonstration of integrating and utilizing the `html-parser` library in a C application.

2. [C++ Demo Project](https://github.com/your-username/html-parser-demo-cpp): This demo project provides a practical demonstration of integrating and utilizing the `html-parser` library in a C++ application.

Feel free to choose the project that best suits your needs and explore how to use the `html-parser` library in your preferred programming language.

## Quick Example

In this section, you'll find a quick example to help you get started with the `html-parser` library. The example provides a clear overview of the library's essential features and how to use them.

### Installation

Before you begin, ensure that you have the `html-parser` library installed. Follow these straightforward steps for installation:

1. **Clone the repository:**
    Use the following command to clone this repository in your project (or favorite) folder:
    ```shell
    git clone https://github.com/florianmarkusse/html-parser.git
    ```

2. **Install CMake:**
   Make sure you have CMake installed on your system. You can find installation instructions [here](https://cmake.org/install/).

3. **Build the Project:**
   Use the following commands inside the root folder of the repository to build the project based on your platform:

   - **For Linux or macOS:**
     If you are on Linux or macOS, you can use the provided `build.sh` script. Run the script with the `-h` flag to view all available build options:
     ```shell
     ./build.sh
     ```
     otherwise, u can simply use:
     ```shell
     cmake -S . -B build/ -D CMAKE_BUILD_TYPE="Release" -D BUILD_SHARED_LIBS="false" -D BUILD_TESTS="false" -D BUILD_BENCHMARKS="false"
     cmake --build build/
     ```

See [this section](#running-benchmarks-%26-tests) for more information on building and running the tests and benchmarks.

### Usage
Here's a comprehensive example showcasing how to use the html-parser library to parse and manipulate an HTML document using C:

```C
#include <flo/html-parser.h>
#include <stdio.h>

int main() {
    // Create the memory arena. This should not be done often.
    flo_html_Arena arena = flo_html_newArena(1U << 27U);
    // Set the jump buffer used to jump to this place of the code in case 
    // of OOM errors during allocations.
    void *jmp_buf[5];
    if (__builtin_setjmp(jmp_buf)) {
        flo_html_destroyArena(&arena);
        FLO_HTML_PRINT_ERROR("OOM in arena!\n");
        return 1;
    }
    arena.jmp_buf = jmp_buf;


    flo_html_Dom *dom = flo_html_createDomFromFile(fileLocation, &arena);
    if (dom == NULL) {
        return 1;
    }

    printf("Before modification\n\n");
    flo_html_printHTML(dom);

    // Find the ID of the <body> element
    flo_html_node_id bodyNodeID = 0;
    if (flo_html_querySelector(FLO_HTML_S("body"), dom, &bodyNodeID, arena) !=
        QUERY_SUCCESS) {
        fprintf(stderr, "Failed to query DOM!\n");
        return 1;
    }

    // Check if the body element has a specific boolean property
    // In other words: "<body add-extra-p-element> ... </body>"
    if (flo_html_hasBoolProp(bodyNodeID, FLO_HTML_S("add-extra-p-element"),
                             dom)) {
        // Append HTML content to the <body> element
        if (!flo_html_appendHTMLFromStringWithQuery(
                FLO_HTML_S("body"), FLO_HTML_S("<p>I am appended</p>"), dom,
                &arena)) {
            fprintf(stderr, "Failed to append to DOM!\n");
            return 1;
        }
    }

    // Print the modified HTML
    printf("After modification\n\n");
    flo_html_printHTML(dom);

    // The created memory arena is automatically destroyed by the operating system.
    // But if you want to do it manually, you can call: flo_html_destroyArena(&arena);
    return 0;
}
```

### Explanation

This example demonstrates how to use the `html-parser` library to parse and manipulate an HTML document using C. Let's break down the code step by step:

- **Memory arena initialization:**
  We initialize an `flo_html_Arena` to manage the memory allocations of the program.

- **DOM Initialization:**
  We create a `flo_html_Dom` structure and parse an HTML file named "test-file.html" using `flo_html_createDomFromFile`. The parsed DOM is stored in the `dom` structure.

- **Querying for the `<body>` Element:**
  We use `querySelector` to find the `<body>` element in the parsed DOM and retrieve its node ID.

- **Checking a Boolean Property:**
  With `flo_html_hasBoolProp`, we check if the `<body>` element has a specific boolean property called "add-extra-p-element."

- **Appending HTML:**
  If the boolean property exists, we append an HTML string `<p>I am appended</p>` to the `<body>` element using `flo_html_appendHTMLFromStringWithQuery`.

- **Printing the Modified HTML:**
  We print the modified HTML, showing the changes made to the document.

This example provides a practical demonstration of the `html-parser` library's capabilities for parsing and manipulating HTML documents in a C program.

## Functionalities

### 1. Parsing HTML
- **Reading an HTML file or string into a `Dom`:** The `html-parser` library provides a straightforward way to parse HTML content, whether it's stored in a file or a string. This feature allows you to create a structured representation of the HTML document.

### 2. Querying and Traversing
- **Querying over the document with CSS queries:** One of the key features of the library is its ability to query and traverse the parsed DOM using CSS queries. This means you can select specific elements or groups of elements within the HTML document based on their attributes, classes, or tags.

### 3. Content Extraction and Modification
- **Reading properties and text content of nodes:** You can extract information from nodes, such as attributes and text content. This is invaluable when you need to retrieve specific data from an HTML document.

- **Modifying nodes:** The library allows you to modify the properties and content of individual nodes, giving you the ability to update the HTML dynamically. For example, you can change the attributes, text, or even add new elements to the document.

### 4. Manipulating the DOM Structure
- **Modifying the DOM structure:** Beyond modifying individual nodes, you can also manipulate the entire DOM structure. This includes adding, removing, and replacing nodes to reshape the document as needed.

### 5. Output Generation
- **Writing out a `Document` to a file in minified HTML:** Once you've made your desired changes to the DOM, the library enables you to generate a new HTML file with the updated content. You can choose to format the output as minified HTML for production use.

These functionalities make the `html-parser` library a versatile tool for working with HTML documents programmatically. Whether you need to scrape data, automate web-related tasks, or transform HTML content, this library provides the tools you need to accomplish your goals efficiently.

## Extending and Building Upon
The provided example is just the tip of the iceberg. You can extend the functionality of your applications by combining the `html-parser` library with other libraries and tools. Here are some ideas:

- **Data Extraction:** Use the library to extract specific data from web pages, such as prices, product details, or news articles.

- **Web Automation:** Combine `html-parser` with web automation frameworks like Selenium to create intelligent web scraping bots.

- **Content Generation:** Dynamically generate HTML content for websites or email templates by programmatically building and modifying the DOM structure.

- **Integration with Other Languages:** Explore ways to use the library in conjunction with other programming languages through interop mechanisms.

Feel free to experiment and innovate with the library to tailor it to your unique use cases. If you have questions or need assistance, you can engage with the community or reach out to the library's maintainers through [GitHub issues](https://github.com/florianmarkusse/html-parser/issues) or other communication channels. Your creativity is the limit when it comes to leveraging the `html-parser` library for web-related tasks.

## Feedback & Assistance
If you encounter any challenges or have suggestions related to the functionalities provided by this library, please do not hesitate to:
- [Open an issue](https://github.com/florianmarkusse/html-parser/issues/new/choose), or;
- [Open a PR](https://github.com/florianmarkusse/html-parser/compare).

We value your input and are committed to improving the project based on your feedback. Moreover, I would be absolutely delighted to see someone using my library :).

## Under the Hood

In this section, we'll delve into how the parser works under the hood to give you a better overview.

### Parsing

First, the HTML string has to be parsed before it can be manipulated. This is accomplished by a state machine that processes tokens one by one. Unlike a strict parser, this process is lenient, meaning it doesn't strictly adhere to the [HTML specification](https://html.spec.whatwg.org/). Instead, it does its best to interpret the input and make sense of it.

#### Node Types

The parser distinguishes between two main node types:

- **Document Node:** Document nodes can further be categorized into two subtypes:
  - **Single Node:** For example: ```<input />```.
  - **Paired Node:** For example: ```<div></div>```.

- **Text Node:** Text nodes simply contain text content, such as: `This is a sentence.`. Text nodes are commonly found within paired nodes, as in ```<p>my paragraph</p>```.

#### (Boolean) Properties
The parser differentiates between two types of properties:
- `Boolean Property`: This is a value on a tag that is either present or not. For example: ```<input required />``` . In this example, `required` is a boolean property on the `input` document node.

- `Property`: This is a key-value pair on a tag. For example: ```<p id="a special id"></p>```. In this example, `id="a special id"` is a property on the `p` document node.

#### Document Structure

The HTML string is parsed into a `flo_html_Dom`. Instead of a traditional tree structure of nodes, the `flo_html_Dom` follows a data-oriented pattern. The `flo_html_Dom` comprises several tables, each serving a specific purpose:

##### Node Table (`nodes`)
- `nodeID` | `nodeType` | `tagID/text` 
  - *Notes:* Whether the third column contains a tagID or text is based on the nodeType of the node. If it is a document node, it contains a tagID. If it is a text node, it contains the pointer to the text.

##### Parent-First-Child Relationship Table (`parentFirstChilds`)
- `parentID` | `childID`

*Represents parent and their first child relationships.*

##### Parent-Child Relationship Table (`parentChilds`)
- `parentID` | `childID`

*Represents parent-child relationships.*

##### Next Node Sequence Table (`nextNodes`)
- `currentNodeID` | `nextNodeID`  

*Tracks the sequence of nodes under the same parent.*

##### Boolean Property Table (`boolprops`)
- `nodeID` | `propID`  

*Records boolean properties of nodes.*

##### Registries
`flo_html_Dom` also holds registries that relate the ID to its actual string value.
- `tagRegistry`: Records all tags seen thusfar. 
- `boolPropRegistry`: Records all boolean properties seen thusfar.
- `propKeyRegistry`: Records all key properties seen thusfar.
- `propValueRegistry`: Records all value properties seen thusfar.

These registries provide a mapping from ID -> string.

##### HashSets
`flo_html_Dom` also holds hashsets to keep track of which tokens have already been parsed:
- `tags`: Records all tags seen thusfar. 
- `boolPropsSet`: Records all boolean properties seen thusfar.
- `propKeys`: Records all key properties seen thusfar.
- `propValues`: Records all value properties seen thusfar.

These hashsets provide a mapping from string -> ID.

#### Explanation

As you can observe, the `flo_html_Dom` does not directly store text content but rather references `IDs`. To retrieve textual content, you use the `ID` to look up the corresponding value in the related `registry`.

For instance, if we have a *node* table entry: `{ 4, NODE_TYPE_DOCUMENT, 5 }`, and we want to find the text representation, we do `tagRegistry[5]`. This yields `div`, for example, as the actual tag. 

And, when parsing a new document node, say `<div></div>`, we first insert in the `tags` hashset. If this value already exists, the ID is merely returned, `5` in this example. Otherwise, `div` is inserted and a new ID is returned.

Why was this decision made instead of storing all data directly in each node?
- It allows nodes with the same tag, boolean property, or property to share the same `ID`.
- It facilitates faster node filtering.
- The library's maintainer, [Florian Markusse](https://github.com/florianmarkusse), wanted to experiment with data-oriented programming and found this approach both challenging and educational.

This design choice enhances performance, reduces memory overhead, and optimizes node filtering. It also provides an opportunity for developers to explore data-oriented programming concepts.

#### Methods to create a DOM

Creating a `flo_html_Dom` is possible with the following methods:

- `flo_html_createDom`: Creates the `flo_html_Dom` based on the provided HTML string. 

- `flo_html_createDomFromFile`: Creates the `flo_html_Dom` based on the provided file location.

- `flo_html_duplicateDom`: Creates the `flo_html_Dom` based on the provided `flo_html_Dom`.


### Querying
After parsing, we can query the `flo_html_Dom` for information that we are looking for. This section is split up into two sections: querying the `flo_html_Dom` and querying the contents of an individual node.
Together, these functions empower you to query the `flo_html_Dom` and individual nodes allowing the user to query the `flo_html_Dom` effectively.

#### Dom

Querying the `flo_html_Dom` is possible with convenience methods similar to querying a web DOM. These methods include:

- `querySelector`: Retrieves the first element in the `flo_html_Dom` that matches a specified CSS selector. It returns a single `flo_html_node_id`.

- `querySelectorAll`: Retrieves all elements in the `flo_html_Dom` that match a specified CSS selector. It returns an array of `flo_html_node_id`s. In the latter case, please remember to `free` this array.

#### Node

The `html-parser` library provides a set of convenient functions to query and retrieve properties and content from individual nodes within the `flo_html_Dom`. These functions allow you to inspect and work with specific attributes and text content of nodes. Here's a brief overview of the available functions:

- `flo_html_hasBoolProp`: Checks if a node has a specified boolean property and returns `true` if the property exists and is true.

- `flo_html_hasPropKey`: Checks if a node has a property with a specific key.

- `flo_html_hasPropValue`: Checks if a node has a property with a specific value.

- `flo_html_hasProperty`: Checks if a node has a property with both a specific key and value.

- `flo_html_getValue`: Retrieves the value of a property associated with a node.

- `flo_html_getTextContent`: Retrieves the text content of a node, storing results in an array of strings. Remember to `free` this array as needed.

### Traversal
After querying, maybe you want to traverse the `flo_html_Dom` to find the first child or the parent of the queries node. Here are some functions to do exactly that!

- `flo_html_getFirstChild`: Retrieves the ID of the first child node of a given node. Returns `0` if there are no child nodes.

- `flo_html_getFirstChildNode`: Returns a pointer to the `flo_html_ParentChild` structure. Returns `NULL` if there are no child nodes.

- `flo_html_getNext`: Retrieves the ID of the next sibling node of a given node. Returns `0` if there are no more sibling nodes.

- `flo_html_getNextNode`: Returns a pointer to the `flo_html_NextNode` structure. Returns `NULL` if there are no more sibling nodes.

- `flo_html_getPrevious`: Retrieves the ID of the previous sibling node of a given node. Returns `0` if there are no previous sibling nodes.

- `flo_html_getPreviousNode`: Returns a pointer to the `flo_html_NextNode` structure. Returns `NULL` if there are no previous sibling nodes.

- `flo_html_getParent`: Retrieves the ID of the parent node of a given node. Returns `0` if there is no parent node.

- `flo_html_getParentNode`: Returns a pointer to the `flo_html_ParentChild` structure. Returns `NULL` if there is no parent node.

- `flo_html_traverseDom`: Traverses the DOM structure from the specified node and returns the ID of the next node. Returns `0` if there are no more nodes to traverse.

- `flo_html_traverseNode`: Traverses the DOM structure of a specific to node with the given ID to traverse and returns the ID of the next node inside that specific node. Returns `0` if there are no more nodes in the specific node.

- `flo_html_getLastNext`: Retrieves the ID of the last next sibling node starting from a given node. Returns `0` if there are no more sibling nodes.

- `flo_html_getLastNextNode`: Returns a pointer to the `flo_html_NextNode` structure representing the last next sibling node starting from a given node. Returns `NULL` if there are no more sibling nodes.


### Modifying

Now that we have some `flo_html_node_id`s after querying and traversing the `flo_html_Dom`, we can modify the `flo_html_Dom` to our heart's content. Again, these functions are split up into two levels: "dom-based" and "node-based". All operations modify the `flo_html_Dom` in place.

#### Dom

Below, all the **append** functions provided. They append a new child to the provided parent node. This library also provides the same functionality to **prepend** and **replaceWith**. Prepending a node adds a new child as the first child node of the provided parent node. Lastly, Replacing a node completely, thus also all its children, does exactly that.

- `flo_html_appendDocumentNodeWithQuery`: Append a `flo_html_DocumentNode` to the DOM using a CSS query. This function appends a `flo_html_DocumentNode` specified by `docNode` to the DOM using the provided CSS query `cssQuery`.

- `flo_html_appendTextNodeWithQuery`: Append a text node to the DOM using a CSS query. This function appends a text node with the specified `text` to the DOM using the provided CSS query `cssQuery`.

- `flo_html_appendHTMLFromStringWithQuery`: Append HTML content from a string to the DOM using a CSS query. This function appends HTML content specified by `htmlString` to the DOM using the provided CSS query `cssQuery`.

- `flo_html_appendHTMLFromFileWithQuery`: Append HTML content from a file to the DOM using a CSS query. This function appends HTML content from the specified `fileLocation` to the DOM using the provided CSS query `cssQuery`.

- `flo_html_appendDocumentNode`: Append a `flo_html_DocumentNode` to the DOM. This function appends a `flo_html_DocumentNode` specified by `docNode` to the DOM.

- `flo_html_appendTextNode`: Append a text node to the DOM. This function appends a text node with the specified `text` to the DOM.

- `flo_html_appendHTMLFromString`: Append HTML content from a string to the DOM. This function appends HTML content specified by `htmlString` to the DOM.

For the sake of brevity, the `prepend...` and `replaceWith...` functions are left out but **are** present in the library. Simply replace `append` with your desired operation.

#### Node

To make changes to specific nodes within the DOM, this library provides a set of functions for adding and updating properties, text content, and tags. These functions allow you to manipulate the HTML elements identified by their unique `flo_html_node_id` within the DOM structure. Below are some key node modification functions:

##### Adding Properties to an HTML Element

- `flo_html_addPropertyToNodeStringsWithLength`: Add a property with a specified key and value to an HTML element. This function takes the `flo_html_node_id` of the target element, the property key, property value, and other necessary parameters.

- `flo_html_addPropertyToNodeStrings`: A simplified version of the above function for adding a single property to an HTML element.

- `flo_html_addBooleanPropertyToNodeStringWithLength`: Add a boolean property to an HTML element, specifying the property key, property value, and length.

- `flo_html_addBooleanPropertyToNodeString`: A simplified version of the above function for adding a single boolean property to an HTML element.

- `flo_html_setPropertyValue`: Set the value of an HTML element's property by specifying the `flo_html_node_id`, property key, and the new value.

- `flo_html_setTextContent`: Set the text content of an HTML element identified by `flo_html_node_id` to the specified text. This function allows you to update the content of an element. Note that this function will remove any child elements this node may have.

- `flo_html_addTextToTextNode`: Add text content to a text node within an HTML element. You can specify whether to append or prepend the text content. 

- `flo_html_setTagOnDocumentNode`: Set the tag for a DocumentNode within the DOM structure. You can specify the tag's start, length, and whether it is paired or not. 

These functions provide a comprehensive set of tools for making precise modifications to the HTML elements within the DOM. You can use them to customize your parsed HTML content to suit your specific needs.

### Printing & Writing
Lastly, after making modifications to the parsed HTML content, you may want to output or print the resulting HTML. This library offers a set of functions to help you achieve this:

- `flo_html_printHTML`: Use this function to print the minified HTML representation. It displays all the elements, tags, and text content in a compact format. This is particularly helpful for inspecting the parsed HTML document directly within your program.

- `flo_html_writeHTMLToFile`: If you wish to save the parsed HTML document to a file, this function is your solution. It writes the minified HTML representation to the specified `filePath`. The function returns a status code to indicate the success or failure of the file-writing operation, making it easy to handle file I/O errors.

- `flo_html_printDomStatus`: This function allows you to print the status of the `flo_html_Dom`. It provides information about node counts, registrations, and other relevant details. It can be a valuable tool for debugging and gaining insights into the structure of the parsed DOM.

These printing and writing functions provide essential utilities for interacting with and exporting the parsed HTML content, whether you need to debug, inspect, or save the modified DOM structure to a file for further use.

## Contributing
It would be amazing if you are willing to contribute to the project. Please look at any issues if they are present or reach out to [the maintainer](https://github.com/florianmarkusse) to collaborate!

### Running Benchmarks & Tests
This repository comes with tests and a simple benchmarking tool included. If you want to run these programs, please follow these steps:

#### Build the Project:
   Use the following commands to build the project based on your platform:

   - **For Linux or macOS:**
     If you are on Linux or macOS, you can use the provided `build.sh` script. Run the script with the `-h` flag to view all available build options:
     ```shell
     ./build.sh -h
     ```

#### Running Tests
```shell
build/tests/html-parser-tests-Release
```

#### Running Benchmarks
```shell
build/benchmarks/html-parser-benchmarks-Release
```

## License

This project is licensed under the GNU-V2.1 License. See the [LICENSE](https://github.com/florianmarkusse/html-parser-demo-c/blob/main/LICENSE) file for details.

---
**NB**: Since this parser is lenient, it can probably also be used to parse XML, or similar markup languages. Be advised, this has not been tested and is not the goal of this project.
