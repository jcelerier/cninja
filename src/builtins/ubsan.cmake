# Enable UndefinedBehaviourSanitizer
cninja_optional(compiler)
string(APPEND CMAKE_C_FLAGS_INIT " -fsanitize=undefined")
string(APPEND CMAKE_CXX_FLAGS_INIT " -fsanitize=undefined")

if("clang" STREQUAL "${CNINJA_COMPILER}")
  string(APPEND CMAKE_C_FLAGS_INIT " -fsanitize=undefined -fsanitize=integer")
  string(APPEND CMAKE_CXX_FLAGS_INIT " -fsanitize=undefined -fsanitize=integer")
endif()
