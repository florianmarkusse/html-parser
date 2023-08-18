cmake_minimum_required(VERSION 3.21)
project(html-parser-tests)

list(APPEND TEST_SOURCE_FILES
    "src/main.c"
    "src/parsing/parsing.c"
    "src/comparisons/comparisons.c"
    "src/querying/querying.c"
)

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(EXECUTABLE_NAME "${PROJECT_NAME}-Debug")
else()
    set(EXECUTABLE_NAME "${PROJECT_NAME}-Release")
endif()

# Define the test executable target
add_executable(${EXECUTABLE_NAME} ${TEST_SOURCE_FILES})

target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
