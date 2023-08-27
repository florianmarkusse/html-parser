cmake_minimum_required(VERSION 3.21)
project(html-parser-benchmarks)

list(APPEND BENCHMARK_SOURCE_FILES
    "src/main.c"
)

set(EXECUTABLE_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

# Define the test executable target
add_executable(${EXECUTABLE_NAME} ${BENCHMARK_SOURCE_FILES})

# Link the test executable with the main project shared library
target_link_libraries(${EXECUTABLE_NAME} PRIVATE ${LIBRARY_NAME})

# Include the "include" directory from the main project, if necessary
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../include)

