cmake_minimum_required(VERSION 3.10)
project(cninja LANGUAGES CXX)
enable_testing()

add_executable(cninja)

target_include_directories(cninja
  PRIVATE
    3rdparty/cxxopts/include
)

target_sources(cninja
  PRIVATE
    src/main.cpp
)

target_compile_options(cninja
  PRIVATE
    $<IF:$<BOOL:${WIN32}>,/std:c++latest,-std=c++1z>
)

install(
  TARGETS
    cninja
  RUNTIME DESTINATION
    bin
)

file(COPY tests DESTINATION .)

set(FLAGS fast small full-lto thin-lto asan ubsan tsan debugmode debugsyms warnings gcc libcxx static examples tests)
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
