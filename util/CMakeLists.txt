project(util)

set(FLO_UTIL_LIBRARY "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

add_library(
    ${FLO_UTIL_LIBRARY}
    STATIC
    "src/file/read.c"
    "src/file/path.c"
    "src/hash/msi/common.c"
    "src/hash/msi/string-set.c"
    "src/hash/msi/uint16-set.c"
    "src/hash/trie/string-set.c"
    "src/hash/trie/string-uint16-map.c"
    "src/hash/trie/uint16-set.c"
    "src/hash/hash-comparison-status.c"
    "src/memory/arena.c"
    "src/memory/buddy-allocator.c"
    "src/memory/pool-allocator.c"
    "src/log.c"
)

set(FLO_UTIL_INCLUDES ${CMAKE_CURRENT_SOURCE_DIR}/include/)
target_include_directories(${FLO_UTIL_LIBRARY} PRIVATE ${FLO_UTIL_INCLUDES})
