# Enable runtime debug checking (e.g. iterator validity checkers)
cninja_optional(libcxx)

set_cache(CMAKE_BUILD_TYPE Debug)

if(libcxx IN_LIST CNINJA_FEATURES)
  # See https://libcxx.llvm.org/docs/DesignDocs/DebugMode.html
  string(APPEND CMAKE_CXX_FLAGS_INIT " -D_LIBCPP_DEBUG=1")
else()
  # See https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode.html
  string(APPEND CMAKE_CXX_FLAGS_INIT " -D_GLIBCXX_DEBUG=1 -D_GLIBCXX_DEBUG_PEDANTIC=1")
endif()

# Note : Windows's stdlib has support for that too,
# but we're mostly concerned with libc++
# https://docs.microsoft.com/en-us/cpp/standard-library/iterator-debug-level

# Boost.MultiIndex comes with similar abilities :
# https://www.boost.org/doc/libs/1_72_0/libs/multi_index/doc/tutorial/debug.html
string(APPEND CMAKE_CXX_FLAGS_INIT " -DBOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING=1")
string(APPEND CMAKE_CXX_FLAGS_INIT " -DBOOST_MULTI_INDEX_ENABLE_SAFE_MODE=1")

# Same for ASIO
string(APPEND CMAKE_CXX_FLAGS_INIT " -DBOOST_ASIO_ENABLE_BUFFER_DEBUGGING=1")
# See also https://www.boost.org/doc/libs/1_73_0/doc/html/boost_asio/using.html

# Not enabled by default but Boost.Spirit also has debugging built-in :
# https://www.boost.org/doc/libs/1_73_0/libs/spirit/classic/doc/debugging.html

# See also
# https://www.boost.org/doc/libs/1_73_0/libs/config/doc/html/boost_config/boost_macro_reference.html
