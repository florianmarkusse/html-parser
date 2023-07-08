cmake_minimum_required(VERSION 3.21)
project(HowCoolJustTheWool)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

set(EXECUTABLE_NAME "html-parser")


if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build (Debug, Release)" FORCE)
endif()
message(STATUS "Selected build type: ${CMAKE_BUILD_TYPE}")

message(STATUS "C-Compiler: ${CMAKE_C_COMPILER}")

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    set(CMAKE_C_FLAGS_DEBUG "-g -O0")  # Add debug flags, disable optimization
else()
    set(CMAKE_C_FLAGS_RELEASE "-O3")  # Enable optimization
endif()

if(CMAKE_BUILD_TYPE STREQUAL "Debug")
    SET(EXECUTABLE_NAME "${EXECUTABLE_NAME}-debug")
else()
    SET(EXECUTABLE_NAME "${EXECUTABLE_NAME}-release")
endif()

LIST(APPEND SOURCE_FILES 
    "src/main.c" 
    "src/utils/file/read.c"
    "src/utils/file/path.c"
    "src/type/tag.c"
    "src/type/data-page.c"
    "src/dom/document.c"
    "src/dom/document-writing.c"
    "src/tokenizer/parse.c" 
    )

add_executable(${EXECUTABLE_NAME} ${SOURCE_FILES})

target_include_directories(${EXECUTABLE_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
