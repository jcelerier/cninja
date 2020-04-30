# Useful compiler warnings
cninja_require(compiler=gcc)

set(temp_warnings
  -Wall
  -Wextra
  -pedantic
  -Wmisleading-indentation
  -Wnon-virtual-dtor
  -Wunused
  -Woverloaded-virtual
  -Werror=return-type
  -Werror=trigraphs
  -Wmissing-field-initializers
  -Wno-unused-parameter
  -Wno-unknown-pragmas
  -Wno-missing-braces
  -Wno-gnu-statement-expression
  -Wno-four-char-constants
  -Wno-cast-align
  -Wno-unused-local-typedef
)

if("gcc" STREQUAL "${CNINJA_COMPILER}")
  list(APPEND temp_warnings
    -Wsuggest-final-types
    -Wsuggest-final-methods
  )
endif()

foreach(warning ${temp_warnings})
  string(APPEND CMAKE_C_FLAGS " ${warning}")
  string(APPEND CMAKE_CXX_FLAGS " ${warning}")
endforeach()

unset(temp_warnings)
