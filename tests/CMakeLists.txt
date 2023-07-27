cmake_minimum_required(VERSION 3.21)
project(html-parser-tests)

list(APPEND TEST_SOURCE_FILES
    "src/main.c"
    "src/comparisons/comparisons.c"
)

if(CMAKE_BUILD_TYPE STREQUAL "debug")
    set(EXECUTABLE_NAME "${PROJECT_NAME}-debug")
else()
    set(EXECUTABLE_NAME "${PROJECT_NAME}-release")
endif()

# Define the test executable target
add_executable(${EXECUTABLE_NAME} ${TEST_SOURCE_FILES})

# Link the test executable with the main project shared library
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})

# Include the "include" directory from the main project, if necessary
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)
