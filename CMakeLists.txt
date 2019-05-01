cmake_minimum_required(VERSION 3.1)
project(tbb_concurrent_queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# some specific compiler definitions
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fcolor-diagnostics" HAS_FCOLOR_DIAGNOSTICS)
if (HAS_FCOLOR_DIAGNOSTICS)
  add_definitions(-fcolor-diagnostics)
endif()
add_definitions(-Wall -Wextra)

add_subdirectory(include)
add_subdirectory(example)
add_subdirectory(test)
