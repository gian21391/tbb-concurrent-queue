cmake_minimum_required(VERSION 3.1)
project(tbb_concurrent_queue LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Options
option(TBBCC_EXAMPLES "Build examples" OFF)
option(TBBCC_TEST "Build tests" OFF)

# some specific compiler definitions
include(CheckCXXCompilerFlag)
check_cxx_compiler_flag("-fcolor-diagnostics" HAS_FCOLOR_DIAGNOSTICS)
if (HAS_FCOLOR_DIAGNOSTICS)
  add_definitions(-fcolor-diagnostics)
endif()
add_definitions(-Wall -Wextra)
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  # using regular Clang or AppleClang
  set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -stdlib=libc++ -lc++abi")
endif()

add_subdirectory(include)

if(TBBCC_EXAMPLES)
  add_subdirectory(example)
endif()

if(TBBCC_TEST)
  enable_testing()
  add_subdirectory(test)
endif()

