cmake_minimum_required(VERSION 3.13)
project(cninja LANGUAGES CXX VERSION 2.0.0)
enable_testing()

# Dependencies
find_package(Boost REQUIRED)

# Create target
add_executable(cninja)

target_include_directories(cninja
  PRIVATE
    3rdparty/cxxopts/include
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
)

target_compile_features(cninja
  PRIVATE
    cxx_std_17
)

target_link_libraries(cninja
  PRIVATE
    Boost::boost
)

# Install target
install(
  TARGETS
    cninja
  RUNTIME DESTINATION
    .
)

# Tests
file(COPY tests DESTINATION .)

set(FLAGS fast small lto=full lto=thin asan ubsan tsan debugmode debugsyms warnings gcc libcxx static examples tests)
foreach(kind static shared executable)
  add_test(
    NAME ${kind}
    COMMAND cninja
    WORKING_DIRECTORY tests/${kind}
  )

  foreach(flag ${FLAGS})
    add_test(
      NAME ${kind}_${flag}
      COMMAND cninja ${flag}
      WORKING_DIRECTORY tests/${kind}
    )

    foreach(flag2 ${FLAGS})
      if(NOT ${flag} STREQUAL ${flag2})
        add_test(
          NAME ${kind}_${flag}_${flag2}
          COMMAND cninja ${flag} ${flag2}
          WORKING_DIRECTORY tests/${kind}
        )
      endif()
    endforeach()
  endforeach()
endforeach()

# Packaging
set(CPACK_INCLUDE_TOPLEVEL_DIRECTORY OFF)
set(CPACK_GENERATOR "ZIP")
include(CPack)
