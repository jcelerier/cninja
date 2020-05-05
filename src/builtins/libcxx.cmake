# Use Clang's libc++ as a standard library (default)

cninja_optional(compiler)

if(CNINJA_COMPILER STREQUAL "clang")
  set(CNINJA_STDLIB libcxx)

  # It's the default everywhere but on Linux anyways
  if(UNIX AND NOT APPLE)
    string(APPEND CMAKE_CXX_FLAGS_INIT " -stdlib=libc++")
    set(CMAKE_CXX_STANDARD_LIBRARIES " -stdlib=libc++")
  endif()
endif()
