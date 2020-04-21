#include "builtins.hpp"

#include <cn/fmt.hpp>
#include <cn/system.hpp>
namespace cn
{

const builtin_map& builtins()
{
  static const builtin_map map = [] {
    builtin_map map;
    map["asan"] =
        R"_(# Enable AddressSanitizer
cninja_optional(gcc)

string(APPEND CMAKE_C_FLAGS_INIT " -fsanitize=address -fno-omit-frame-pointer")
string(APPEND CMAKE_CXX_FLAGS_INIT " -fsanitize=address -fno-omit-frame-pointer")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -fsanitize=address -fno-omit-frame-pointer")
string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -fsanitize=address -fno-omit-frame-pointer")
if("gcc" IN_LIST CNINJA_FEATURES)
  string(APPEND CMAKE_CXX_FLAGS_INIT  "-D_GLIBCXX_SANITIZE_VECTOR")
endif()
)_";

    map["ubsan"] =
        R"_(# Enable UndefinedBehaviourSanitizer
string(APPEND CMAKE_C_FLAGS_INIT " -fsanitize=undefined -fsanitize=integer")
string(APPEND CMAKE_CXX_FLAGS_INIT " -fsanitize=undefined -fsanitize=integer")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -fsanitize=undefined -fsanitize=integer")
string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -fsanitize=undefined -fsanitize=integer")
)_";

    map["tsan"] =
        R"_(# Enable ThreadSanitizer. Incompatible with asan / ubsan.
string(APPEND CMAKE_C_FLAGS_INIT " -fsanitize=thread")
string(APPEND CMAKE_CXX_FLAGS_INIT " -fsanitize=thread")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -fsanitize=thread")
string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -fsanitize=thread")
)_";

    map["gcc"] =
        R"_(# Use GCC as a compiler
set(CMAKE_C_COMPILER gcc)
set(CMAKE_CXX_COMPILER g++)
)_";
    map["clang"] = fmt::format(
        R"_(# Use Clang as a compiler (default)
set(CMAKE_C_COMPILER {})
set(CMAKE_CXX_COMPILER {})
)_",
        sys.clang_binary,
        sys.clangpp_binary);

    map["libcxx"] =
        R"_(# Use Clang's libc++ as a standard library (default)
string(APPEND CMAKE_CXX_FLAGS " -stdlib=libc++")
set(CMAKE_CXX_STANDARD_LIBRARIES " -stdlib=libc++")
)_";

    map["fast"] =
        R"_(# Creates a build optimized for this machine
cninja_require(lto=full)
cninja_optional(debugsyms)
string(APPEND CMAKE_C_FLAGS_INIT " -Ofast -march=native")
string(APPEND CMAKE_CXX_FLAGS_INIT " -Ofast -march=native")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Ofast -march=native -Wl,--icf=all -Wl,-O3")
string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -Ofast -march=native -Wl,--icf=all -Wl,-O3")
if(NOT "debugsyms" IN_LIST CNINJA_FEATURES)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,--strip-all")
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -Wl,--strip-all")
endif()
    )_";
    map["small"] =
        R"_(# Creates a small build
cninja_require(lto=full)
cninja_optional(debugsyms)
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,--icf=all -Wl,--strip-all -Wl,-O3")
string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -Wl,--icf=all -Wl,--strip-all -Wl,-O3")
if(NOT "debugsyms" IN_LIST CNINJA_FEATURES)
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,--strip-all")
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -Wl,--strip-all")
endif()
    )_";

    map["lto"] =
        R"_(# Enable -flto={thin,full}. E.g. pass lto=full.
set(CNINJA_LTO_KIND "%lto%")
string(APPEND CMAKE_C_FLAGS_INIT " -flto=%lto% -fwhole-program-vtables")
string(APPEND CMAKE_CXX_FLAGS_INIT " -flto=%lto% -fwhole-program-vtables")
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -flto=%lto% -fwhole-program-vtables")
string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -flto=%lto% -fwhole-program-vtables")

# Will only be available in clang-11:
# -fvirtual-function-elimination
)_";

    map["debugmode"] =
        R"_(# Enable runtime debug checking (e.g. iterator validity checkers)
set(CMAKE_BUILD_TYPE Debug)

if(libcxx IN_LIST CNINJA_FEATURES)
  # See https://libcxx.llvm.org/docs/DesignDocs/DebugMode.html
  string(APPEND CMAKE_CXX_FLAGS " -D_LIBCPP_DEBUG=1")
else()
  # See https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode.html
  string(APPEND CMAKE_CXX_FLAGS " -D_GLIBCXX_DEBUG=1 -D_GLIBCXX_DEBUG_PEDANTIC=1")
endif()

# Note : Windows's stdlib has support for that too,
# but we're mostly concerned with libc++
# https://docs.microsoft.com/en-us/cpp/standard-library/iterator-debug-level

# Boost.MultiIndex comes with similar abilities :
# https://www.boost.org/doc/libs/1_72_0/libs/multi_index/doc/tutorial/debug.html
string(APPEND CMAKE_CXX_FLAGS " -DBOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING=1")
string(APPEND CMAKE_CXX_FLAGS " -DBOOST_MULTI_INDEX_ENABLE_SAFE_MODE=1")
)_";
    map["debugsyms"] =
        R"_(# Enable generation of debug symbols

# -g3 allows debugging into macros unlike the default of -g
string(APPEND CMAKE_C_FLAGS_INIT " -g3")
string(APPEND CMAKE_CXX_FLAGS_INIT " -g3")
)_";
    map["warnings"] =
        R"_(# More compiler warnings
)_";
    map["era"] =
        R"_(# Oldest compatible system. Example: era=winxp/win7/win10 (on Windows), era=10.14 (on macOS)
if(APPLE)
  set(CMAKE_OSX_DEPLOYMENT_TARGET "%era%")
elseif(WIN32)
  string(TOUPPER "%era%" WIN32_WINNT_MACRO)
  string(APPEND CMAKE_C_FLAGS_INIT " -D_WIN32_WINNT_=${WIN32_WINNT_MACRO}")
  string(APPEND CMAKE_CXX_FLAGS_INIT " -D_WIN32_WINNT_=${WIN32_WINNT_MACRO}")
endif()
)_";

    map["lld"] =
        R"_(# Use lld as a linker when available (Windows & Linux). Super fast. Default.
if(NOT APPLE)
  # LLD is a much faster linker : https://lld.llvm.org
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -fuse-ld=lld")
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -fuse-ld=lld")

  # In conjunction with ffunction-sections / fdata-sections, removes unused code
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -Wl,--gc-sections")
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,--gc-sections")

  # Make all platforms behave like Windows, which is in itself terrible but will
  # spare you trying to debug why dynamic_cast of inline classes across DLLs
  # doesn't work in MSW - simply don't write code assuming this works anywhere.
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -Bsymbolic -Bsymbolic-functions")
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Bsymbolic -Bsymbolic-functions")
endif()

if(NOT WIN32 AND NOT APPLE)
  # Make linking faster
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -Wl,--threads")
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,--threads")

  # Make debugging faster
  string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -Wl,--gdb-index")
  string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -Wl,--gdb-index")
endif()
)_";

    map["static"] =
        R"_(# Create static binaries whenever possible.
# Produce fully static executables
cninja_optional(libcxx)
string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -static-libgcc -static-libstdc++ -static")
string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -static-libgcc -static-libstdc++ -static")

if(libcxx IN_LIST CNINJA_FEATURES)
  string(APPEND CMAKE_CXX_STANDARD_LIBRARIES " -stdlib=libc++ -lc++abi -lpthread")
endif()
)_";
    map["examples"] =
        R"_(# Build examples (if any)
set(BUILD_EXAMPLES ON)
)_";
    map["no-examples"] =
        R"_(# Don't build examples
set(BUILD_EXAMPLE OFF)
set(BUILD_EXAMPLES OFF)
)_";
    map["tests"] =
        R"_(# Build tests (if any)
# Official CMake variable name for tests
set(BUILD_TESTING ON)
)_";
    map["no-tests"] =
        R"_(# Don't build tests
# Disable macro names commonly used
set(BUILD_TESTING OFF)
set(WITH_TESTS OFF)
set(BUILD_TEST OFF)
set(BUILD_TESTS OFF)
)_";

    map["visibility"] =
        R"_(# Hide all symbols (default). Use GenerateExportHeader !
set(CMAKE_C_VISIBILITY_PRESET hidden)
set(CMAKE_CXX_VISIBILITY_PRESET hidden)
set(CMAKE_VISIBILITY_INLINES_HIDDEN 1)
)_";

    map["linkerwarnings"] =
        R"_(# Enforce existence of linked-to functions at compile time

if(APPLE)
  set(temp_LINKER_WARNINGS "-Wl,-fatal_warnings -Wl,-undefined,dynamic_lookup")
else()
  set(temp_LINKER_WARNINGS
       "-Wl,-z,defs \
        -Wl,-z,now \
        -Wl,--unresolved-symbols,report-all \
        -Wl,--warn-unresolved-symbols \
        -Wl,--no-undefined \
        -Wl,--no-allow-shlib-undefined \
        -Wl,--no-allow-multiple-definition \
  ")
endif()

string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " ${temp_LINKER_WARNINGS}")
string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " ${temp_LINKER_WARNINGS}")
unset(temp_LINKER_WARNINGS)
)_";
    // Default build phases
    map["start"] =
        R"_(
cmake_policy(VERSION 3.9)
)_";

    map["pre"] =
        R"_(
cninja_require(start)
)_";

    map["default"] =
        R"_(
cninja_require(pre)
cninja_require(clang)
cninja_require(lld)
cninja_require(visibility)
cninja_require(linkerwarnings)

# -pipe: Potentially makes the build faster
# -ffunction-sections, etc... : Allows to discard unused code more easily with --gc-sections
string(APPEND CMAKE_C_FLAGS_INIT " -pipe -ffunction-sections -fdata-sections")
string(APPEND CMAKE_CXX_FLAGS_INIT " -pipe -ffunction-sections -fdata-sections")

if(WIN32)
  # Remove obnoxious default <windows.h> features
  # - NOMINMAX: min() / max() macros
  # - _CRT_SECURE_NO_WARNINGS: secure versions are not portable, don't use them
  # - WIN32_LEAN_AND_MEAN: makes including <windows.h> faster
  string(APPEND CMAKE_C_FLAGS_INIT " -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN")
  string(APPEND CMAKE_CXX_FLAGS_INIT " -DNOMINMAX -D_CRT_SECURE_NO_WARNINGS -DWIN32_LEAN_AND_MEAN")
endif()

# Don't try to install in protected directories by default
set(CMAKE_INSTALL_PREFIX install)

# Some libraries expect -fPIC
set(CMAKE_POSITION_INDEPENDENT_CODE 1)

# Useful for running various tools, integrations in IDEs...
set(CMAKE_EXPORT_COMPILE_COMMANDS 1)

# We are in 2020
set(CMAKE_CXX_STANDARD 17)

# If you CI run looks like
# $ cmake --build .
# $ cmake --build . --target install
# this will make it faster:
set(CMAKE_SKIP_INSTALL_ALL_DEPENDENCY 1)

)_";

    map["post"] =
        R"_(
cninja_require(default)

if(NOT CMAKE_BUILD_TYPE)
  if("fast" IN_LIST CNINJA_FEATURES)
    if("debugsyms" IN_LIST CNINJA_FEATURES)
      set(CMAKE_BUILD_TYPE RelWithDebInfo)
    else()
      set(CMAKE_BUILD_TYPE Release)
    endif()
  else()
    if("small" IN_LIST CNINJA_FEATURES)
      set(CMAKE_BUILD_TYPE MinSizeRel)
      if("debugsyms" IN_LIST CNINJA_FEATURES)
        string(APPEND CMAKE_C_FLAGS_INIT " -g")
        string(APPEND CMAKE_CXX_FLAGS_INIT " -g")
      else()
        string(APPEND CMAKE_EXE_LINKER_FLAGS_INIT " -g0 -s -Wl,-s -Wl,--icf=all -Wl,--strip-all")
        string(APPEND CMAKE_SHARED_LINKER_FLAGS_INIT " -g0 -s -Wl,-s -Wl,--icf=all -Wl,--strip-all")
      endif()
    else()
      if("debugsyms" IN_LIST CNINJA_FEATURES)
        set(CMAKE_BUILD_TYPE RelWithDebInfo)
      else()
        set(CMAKE_BUILD_TYPE Release)
      endif()
    endif()
  endif()
endif()
)_";

    map["finish"] =
        R"_(
cninja_require(post)
)_";

    return map;
  }();
  return map;
}
}
