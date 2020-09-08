cmake_minimum_required(VERSION 3.13)
project(cninja
  LANGUAGES CXX
  VERSION 3.7.0
  HOMEPAGE_URL "https://github.com/jcelerier/cninja"
  DESCRIPTION "Opinionated CMake config manager"
)

enable_testing()

set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Dependencies
find_package(Boost REQUIRED)

# Create target
add_executable(cninja)

target_include_directories(cninja
  PRIVATE
    3rdparty/fmt/include
    3rdparty/filesystem/include

    src/
)

target_sources(cninja
  PRIVATE
    src/main.cpp

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

target_compile_features(cninja
  PRIVATE
    cxx_std_17
)

target_link_libraries(cninja
  PRIVATE
    Boost::boost
)

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
file(COPY tests DESTINATION .)

set(FLAGS
  fast
  small
  lto=full
  lto=thin
  debugmode debugsyms
  compiler=clang
  libcxx
  plain
  lld
  warnings
  linkerwarnings
  visibility
  static
)

# No point in testing GCC outside of Linux
if(UNIX AND NOT APPLE)
  list(APPEND FLAGS
    compiler=gcc
  )
endif()

if(NOT WIN32)
  list(APPEND FLAGS
    asan ubsan tsan
  )
endif()
foreach(kind static shared executable)
  add_test(
    NAME ${kind}
    COMMAND cninja
    WORKING_DIRECTORY tests/${kind}
  )
  set_tests_properties(${kind} PROPERTIES
    FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed"
  )

  foreach(flag ${FLAGS})
    add_test(
      NAME ${kind}_${flag}
      COMMAND cninja ${flag}
      WORKING_DIRECTORY tests/${kind}
    )
    set_tests_properties(${kind}_${flag} PROPERTIES
      FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed"
    )

    foreach(flag2 ${FLAGS})
      if(NOT ${flag} STREQUAL ${flag2})
        add_test(
          NAME ${kind}_${flag}_${flag2}
          COMMAND cninja ${flag} ${flag2}
          WORKING_DIRECTORY tests/${kind}
        )
        set_tests_properties(${kind}_${flag}_${flag2} PROPERTIES
          FAIL_REGULAR_EXPRESSION "[^a-z]Error;ERROR;Failed"
        )
      endif()
    endforeach()
  endforeach()
endforeach()

# Packaging
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP")
include(CPack)
