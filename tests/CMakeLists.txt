cmake_minimum_required(VERSION 3.21)
project(html-parser-tests)

list(APPEND TEST_SOURCE_FILES
    "src/main.c"
    "src/test.c"
    "src/dom/parsing/parsing.c"
    "src/dom/comparing/comparing.c"
    "src/dom/querying/querying.c"
    "src/dom/querying/querySelectorAll.c"
    "src/dom/querying/querySelector.c"
    "src/node/querying/querying.c"
    "src/node/querying/bool-queries.c"
    "src/node/querying/char-queries.c"
)

set(EXECUTABLE_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

# Define the test executable target
add_executable(${EXECUTABLE_NAME} ${TEST_SOURCE_FILES})

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
