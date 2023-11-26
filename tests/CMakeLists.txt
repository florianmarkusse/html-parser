cmake_minimum_required(VERSION 3.18)
project(html-parser-tests)

list(APPEND TEST_SOURCE_FILES
    "src/main.c"
    "src/test.c"
    "src/comparison-test.c"
    "src/integration-test.c"
    "src/dom/parsing/parsing.c"
    "src/dom/comparing/comparing.c"
    "src/dom/appending/appending.c"
    "src/dom/prepending/prepending.c"
    "src/dom/replacing/replacing.c"
    "src/dom/querying/querying.c"
    "src/dom/deleting/deleting.c"
    "src/dom/duplicating/duplicating.c"
    "src/dom/querying/querySelectorAll.c"
    "src/dom/querying/querySelector.c"
    "src/node/querying/querying.c"
    "src/node/modifying/modifying.c"
    "src/node/deleting/deleting.c"
    "src/node/querying/bool-queries.c"
    "src/node/querying/char-queries.c"
    "src/node/querying/array-queries.c"
)

set(EXECUTABLE_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

add_executable(${EXECUTABLE_NAME} ${TEST_SOURCE_FILES})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

# Libraries
# set(LIBRARY_DIR ${CMAKE_SOURCE_DIR}/../libs) <-- Unnecessary since it is set by parent CMakeLists.txt
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${FLO_HTML_PARSER_INCLUDES})
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_DIR}/util/include)

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_DIR}/util/build/libutil-Release.a)
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${FLO_HTML_PARSER_LIBRARY})

# Compilation options
target_compile_options(${EXECUTABLE_NAME} PRIVATE
    $<$<CONFIG:Debug>:-O0 -g3>
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Profiling>:-O2 -pg>
)

target_compile_definitions(${EXECUTABLE_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:>
    $<$<CONFIG:Profiling>:>
)


