cmake_minimum_required(VERSION 3.18)
project(html-parser-benchmarks)

list(APPEND BENCHMARK_SOURCE_FILES
    "src/main.c"
)

set(EXECUTABLE_NAME "${PROJECT_NAME}-${CMAKE_BUILD_TYPE}")

add_executable(${EXECUTABLE_NAME} ${BENCHMARK_SOURCE_FILES})
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
