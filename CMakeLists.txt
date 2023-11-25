cmake_minimum_required(VERSION 3.18)
project(html-parser)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

### Options ###
option(BUILD_SHARED_LIBS "Build shared libraries instead of static libraries" OFF)
option(CMAKE_BUILD_TYPE "Build type (Debug, Release, Profiling)" "Release")
option(BUILD_TESTS "Build the test executable" OFF)
option(BUILD_BENCHMARKS "Build the benchmark executable" OFF)

### Flags ###
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wall -Wextra -Wconversion -Wno-sign-conversion -Wno-missing-field-initializers -Wdouble-promotion -Wvla -W")

if (CMAKE_BUILD_TYPE STREQUAL "Profiling")
    SET(CMAKE_C_FLAGS "${CMAKE_CXX_FLAGS} -pg")
    SET(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -pg")
    SET(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -pg")
endif()

set(LIBRARY_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

if(BUILD_SHARED_LIBS)
    set(LIBRARY_TYPE "SHARED")
else()
    set(LIBRARY_TYPE "STATIC")
endif()
add_library(${LIBRARY_NAME} ${LIBRARY_TYPE}
    "src/dom/dom-util.c"
    "src/dom/dom.c"
    "src/dom/writing.c"
    "src/dom/traversal.c"
    "src/dom/reading/reading.c"
    "src/dom/reading/reading-util.c"
    "src/dom/modification.c"
    "src/parser.c" 
    "src/dom/comparison.c"
    "src/dom/appendix.c"
    "src/dom/prependix.c"
    "src/dom/replacement.c"
    "src/dom/deletion.c"
    "src/dom/query/query.c"
    "src/dom/query/query-util.c"
    "src/dom/query/msi-uint16.c"
    "src/dom/util/string-auto-uint16-map.c"

    "util/src/file/read.c"
    "util/src/file/path.c"
    "util/src/log.c"
    "util/src/hash/msi/common.c"
    "util/src/hash/msi/string-set.c"
    "util/src/hash/msi/uint16-set.c"
    "util/src/hash/hash-comparison-status.c"
    "util/src/memory/arena.c"
    "util/src/memory/buddy-allocator.c"
    "util/src/memory/pool-allocator.c"
)

target_compile_options(${LIBRARY_NAME} PRIVATE
    $<$<CONFIG:Debug>:-O0 -g3>
    $<$<CONFIG:Release>:-O3>
    $<$<CONFIG:Profiling>:-O2 -pg>
)

target_compile_definitions(${LIBRARY_NAME} PRIVATE
    $<$<CONFIG:Debug>:DEBUG>
    $<$<CONFIG:Release>:>
    $<$<CONFIG:Profiling>:>
)

set(FLO_HTML_INCLUDES
    ${CMAKE_CURRENT_SOURCE_DIR}/include/
    ${CMAKE_CURRENT_SOURCE_DIR}/util/include/
)

target_include_directories(${LIBRARY_NAME} PRIVATE ${FLO_HTML_INCLUDES})

if(${BUILD_TESTS} AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/tests/CMakeLists.txt")
    add_subdirectory(tests)
endif()

if(${BUILD_BENCHMARKS} AND EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/benchmarks/CMakeLists.txt")
    add_subdirectory(benchmarks)
endif()
