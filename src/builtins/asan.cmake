# Enable AddressSanitizer
cninja_optional(compiler)

string(APPEND CMAKE_C_FLAGS_INIT " -fsanitize=address -fno-omit-frame-pointer")
string(APPEND CMAKE_CXX_FLAGS_INIT " -fsanitize=address -fno-omit-frame-pointer")

if("gcc" STREQUAL "${CNINJA_COMPILER}")
  string(APPEND CMAKE_CXX_FLAGS_INIT  " -D_GLIBCXX_SANITIZE_VECTOR")
endif()
