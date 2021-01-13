cmake_minimum_required(VERSION 3.13)
project(cninja
  LANGUAGES CXX
  VERSION 3.7.6
  HOMEPAGE_URL "https://github.com/jcelerier/cninja"
  DESCRIPTION "Opinionated CMake config manager"
)

enable_testing()

if(CMAKE_VERSION VERSION_GREATER 3.19)
  cmake_policy(SET CMP0110 OLD)
endif()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Dependencies
find_package(Boost REQUIRED)
find_package(Catch2)

# Create target
add_library(cn STATIC)

target_include_directories(cn
  PUBLIC
    3rdparty/fmt/include
    3rdparty/filesystem/include

    src/
)

target_sources(cn
  PRIVATE
    src/cn/builtins.hpp
    src/cn/check.hpp
    src/cn/config.hpp
    src/cn/fmt.hpp
    src/cn/fs.hpp
    src/cn/graph.hpp
    src/cn/generate.hpp
    src/cn/options.hpp
    src/cn/system.hpp
    src/cn/util.hpp

    src/cn/builtins.cpp
    src/cn/check.cpp
    src/cn/config.cpp
    src/cn/fs.cpp
    src/cn/generate.cpp
    src/cn/graph.cpp
    src/cn/options.cpp

    src/builtins/asan.cmake
    src/builtins/clang.cmake
    src/builtins/compiler.cmake
    src/builtins/debugmode.cmake
    src/builtins/debugsplit.cmake
    src/builtins/debugsyms.cmake
    src/builtins/default.cmake
    src/builtins/earlybinding.cmake
    src/builtins/era.cmake
    src/builtins/examples.cmake
    src/builtins/fast.cmake
    src/builtins/finish.cmake
    src/builtins/gcc.cmake
    src/builtins/libcxx.cmake
    src/builtins/linker.cmake
    src/builtins/linkerwarnings.cmake
    src/builtins/lld.cmake
    src/builtins/lto.cmake
    src/builtins/no-examples.cmake
    src/builtins/no-tests.cmake
    src/builtins/plain.cmake
    src/builtins/post.cmake
    src/builtins/pre.cmake
    src/builtins/small.cmake
    src/builtins/start.cmake
    src/builtins/static.cmake
    src/builtins/tests.cmake
    src/builtins/tsan.cmake
    src/builtins/ubsan.cmake
    src/builtins/visibility.cmake
    src/builtins/warnings.cmake
)

target_compile_features(cn
  PUBLIC
    cxx_std_17
)

target_link_libraries(cn
  PUBLIC
    Boost::boost
)

add_executable(cninja src/main.cpp)
target_link_libraries(cninja PRIVATE cn)

# Generate the builtins header
include(cmake/generate-builtins.cmake)

# Install target
install(
  TARGETS
    cninja
  RUNTIME DESTINATION
    bin
)

# Tests
include(cmake/setup-tests.cmake)
if(TARGET Catch2::Catch2)
  add_executable(graph_test tests/unit/graph.cpp)
  target_link_libraries(graph_test PRIVATE cn Catch2::Catch2)

  include(CTest)
  include(Catch)
  catch_discover_tests(graph_test)
endif()

# Packaging
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP")
include(CPack)
