cmake_minimum_required(VERSION 4.1)

project(draftMaker LANGUAGES CXX)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

file(GLOB SRC_FILES "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc")

add_executable(draftMaker "interface.cc" ${SRC_FILES})

target_include_directories(draftMaker PRIVATE "${CMAKE_CURRENT_SOURCE_DIR}/include")

target_compile_options(draftMaker PRIVATE
    -Wall
    -Wextra
    -Wpedantic
)

include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-std=c++26" COMPILER_SUPPORTS_CXX26)

if(COMPILER_SUPPORTS_CXX26)
  target_compile_definitions(draftMaker PRIVATE COMPILEDWITHC23)
  message(STATUS "Using C++26")
else()
  message(FATAL_ERROR "Your compiler does not support C++26")
endif()

set(CMAKE_CXX_FLAGS_DEBUG "-g -O0")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")


set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -Wextra -Wpedantic")
