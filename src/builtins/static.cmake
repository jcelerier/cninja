# Create static binaries whenever possible (that is, everywhere but macOS).
cninja_require(pre)
cninja_optional(libcxx)
cninja_optional(asan)

# Following options are for building & looking for static libraries
set_cache(BUILD_SHARED_LIBS OFF)
set_cache(CMAKE_FIND_LIBRARY_SUFFIXES .a)
set_cache(CMAKE_LINK_SEARCH_END_STATIC ON)

if(APPLE)
  add_linker_flags(" -static-libstdc++")
else()
  add_linker_flags(" -static-libgcc -static-libstdc++")
endif()

if(CNINJA_STDLIB STREQUAL "libcxx" AND NOT WIN32)
  add_linker_flags(" -lc++abi -pthread")
endif()

if(APPLE)
  add_linker_flags(" -pthread")
elseif(UNIX AND NOT WIN32)
  # See https://stackoverflow.com/a/31265512/1495627
  add_linker_flags(" -pthread -Wl,--whole-archive -lpthread -Wl,--no-whole-archive")
endif()

# See https://stackoverflow.com/a/5259427/1495627
if(NOT APPLE)
  # ASAN requires dynamic binaries:
  # https://github.com/google/sanitizers/wiki/AddressSanitizer
  if(NOT asan IN_LIST CNINJA_FEATURES)
    set_cache(CMAKE_LINK_SEARCH_START_STATIC ON)
    add_linker_flags(" -static")
  endif()
endif()
