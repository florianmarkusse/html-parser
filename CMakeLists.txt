cmake_minimum_required(VERSION 3.21)
project(html-parser)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "release" CACHE STRING "Choose the type of build (debug, release)" FORCE)
endif()

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(CMAKE_C_FLAGS_DEBUG "-g -O0")  # Add debug flags, disable optimization
set(CMAKE_C_FLAGS_RELEASE "-O3")  # Enable optimization

set(LIBRARY_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

# Create a shared library for the main project
add_library(${LIBRARY_NAME} SHARED
    "src/utils/file/read.c"
    "src/utils/file/path.c"
    "src/type/element/elements.c"
    "src/type/data/data-functions.c"
    "src/dom/document.c"
    "src/dom/document-writing.c"
    "src/parse/parse.c" 
    "src/dom/document-user.c"
    "src/dom/comparison/document-comparison.c"
    "src/dom/querying/document-querying.c"
    "src/dom/querying/document-querying-util.c"
    "src/dom/document-utils.c"
)

if (CMAKE_BUILD_TYPE STREQUAL "debug")
    add_definitions(-DENABLE_DEBUG_MACRO)
endif()
target_include_directories(${LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

if(RUN_TESTS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()

if(RUN_BENCHMARKS AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/CMakeLists.txt")
    add_subdirectory(benchmarks)
endif()
