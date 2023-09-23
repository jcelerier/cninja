file(COPY tests DESTINATION .)

set(FLAGS
  fast
  small
  lto=full
  lto=thin
  debugmode debugsyms
  compiler=clang
  libcxx
  default=plain
  post=plain
  lld
  warnings
  linker-warnings
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
