# Create static binaries whenever possible (that is, everywhere but macOS).
cninja_require(pre)
cninja_optional(libcxx)
cninja_optional(asan)
set(BUILD_SHARED_LIBS OFF)
set(CMAKE_FIND_LIBRARY_SUFFIXES .a)

if(APPLE)
  add_linker_flags(" -static-libstdc++")
else()
  add_linker_flags(" -static-libgcc -static-libstdc++")
endif()

set(CMAKE_LINK_SEARCH_END_STATIC ON)

if(CNINJA_STDLIB STREQUAL "libcxx" AND NOT WIN32)
  string(APPEND CMAKE_CXX_STANDARD_LIBRARIES " -lc++abi -pthread")
endif()

# See https://stackoverflow.com/a/5259427/1495627
if(NOT APPLE)
  # ASAN requires dynamic binaries:
  # https://github.com/google/sanitizers/wiki/AddressSanitizer
  if(NOT asan IN_LIST CNINJA_FEATURES)
    set(CMAKE_LINK_SEARCH_START_STATIC ON)
    add_linker_flags(" -static")
  endif()
endif()
