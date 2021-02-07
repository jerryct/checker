cmake_minimum_required(VERSION 3.10)

project(checker LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_EXTENSIONS Off)

find_package(Clang REQUIRED CONFIG NO_DEFAULT_PATH PATHS "/home/jerry/playground/llvm-project/_install/lib/cmake/clang")

add_compile_options(
  "-fno-exceptions"
  "-fno-omit-frame-pointer"
  "-fno-rtti"
  "-g"
  "-Wall"
  "-Wextra"
  "-Wpedantic"
  "-Werror"
  "-D__STDC_LIMIT_MACROS"
  "-D__STDC_CONSTANT_MACROS"
  "-D__STDC_FORMAT_MACROS"
)

add_executable(checker main.cpp)
target_include_directories(checker SYSTEM PRIVATE ${CLANG_INCLUDE_DIRS} ${LLVM_INCLUDE_DIRS})
target_link_libraries(checker PRIVATE clangTooling clangASTMatchers)
