cmake_minimum_required(VERSION 3.21)
project(html-parser)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

### Options ###
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries" OFF)
option(CMAKE_BUILD_TYPE "Build type (Debug, Release)" "Release")
option(BUILD_TESTS "Build the test executable" OFF)
option(BUILD_BENCHMARKS "Build the benchmark executable" OFF)

### Flags ###
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall")
set(DEBUG_FLAGS "-g -O0")
set(RELEASE_FLAGS "-O3")

set(LIBRARY_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

if(BUILD_SHARED_LIBS)
    set(LIBRARY_TYPE "SHARED")
else()
    set(LIBRARY_TYPE "STATIC")
endif()
add_library(${LIBRARY_NAME} ${LIBRARY_TYPE}
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


target_compile_options(${LIBRARY_NAME} PRIVATE
    "$<$<CONFIG:Debug>:${DEBUG_FLAGS}>"
    "$<$<CONFIG:Release>:${RELEASE_FLAGS}>"
)

target_compile_definitions(${LIBRARY_NAME} PRIVATE
    "$<$<CONFIG:Debug>:ENABLE_DEBUG_MACRO>"
    "$<$<CONFIG:Release>:>"
)

target_include_directories(${LIBRARY_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

if(${BUILD_TESTS} AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()

if(${BUILD_BENCHMARKS} AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/CMakeLists.txt")
    add_subdirectory(benchmarks)
endif()
