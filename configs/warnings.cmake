set(COMMON_WARNINGS 
# Default set of warning groups
 -Wall
 -Wextra
 -pedantic
 -Wmisleading-indentation
 -Wnon-virtual-dtor
 -Wunused
 -Woverloaded-virtual
 -Wmissing-field-initializers

# Omitting a return is UB and almost always an error,
# make it fail explicitely
 -Werror=return-type
 -Werror=trigraphs

# Disable some too noisy ones
 -Wno-unused-parameter
 -Wno-unknown-pragmas
 -Wno-missing-braces
 -Wno-gnu-statement-expression
 -Wno-four-char-constants
 -Wno-cast-align
 -Wno-unused-local-typedef 
)

string(APPEND CMAKE_C_FLAGS_INIT " ${COMMON_WARNINGS}")
string(APPEND CMAKE_CXX_FLAGS_INIT " ${COMMON_WARNINGS}")

unset(COMMON_WARNINGS)