#include <iostream>
#include <filesystem>
#include <cxxopts.hpp>
namespace
{
constexpr bool is_libcxx_default() noexcept
{
  // We'll assume that libc++ is the default for clang
  // everywhere but on Linux.
#if defined(_LIBCPP_VERSION) || !defined(__linux__)
  return true;
#endif
  return false;
}
std::string uppercase(std::string s) noexcept
{
  std::transform(
        s.begin(), s.end(),
        s.begin(),
        [] (unsigned char c) { return std::toupper(c); }
  );
  return s;
}
}

constexpr const struct System
{
  static constexpr bool os_apple{
#if defined(__APPLE__)
    true
#endif
  };

  static constexpr bool os_linux{
#if defined(__linux__)
    true
#endif
  };

  static  constexpr bool os_windows{
#if defined(_WIN32)
    true
#endif
  };
} sys;

struct Options
{
  bool fast{};
  bool small{};

  bool full_lto{};
  bool thin_lto{};

  bool asan{};
  bool ubsan{};
  bool tsan{};

  bool staticbuild{};
  bool profile{};
  bool coverage{};

  bool debugsyms{};
  bool debugmode{};
  bool warnings{};

  bool gcc{};
  bool libcxx{is_libcxx_default()};

  bool examples{};
  bool tests{};

  std::string target_era{};
};

int main(int argc, char** argv) try
{
  Options options{};

  cxxopts::Options args{argv[0], " - an opinionated CMake frontend"};
  args.positional_help("[optional args]")
      .show_positional_help()
  ;

  args.add_options()
      ("fast", "Creates a build optimized for this machine", cxxopts::value<bool>(options.fast))
      ("small", "Creates a small build", cxxopts::value<bool>(options.small))

      ("full-lto", "Full LTO", cxxopts::value<bool>(options.full_lto))
      ("lto", "Thin LTO", cxxopts::value<bool>(options.thin_lto))
      ("thin-lto", "Thin LTO", cxxopts::value<bool>(options.thin_lto))

      ("asan", "AddressSanitizer", cxxopts::value<bool>(options.asan))
      ("ubsan", "UndefinedBehaviourSanitizer", cxxopts::value<bool>(options.ubsan))
      ("tsan", "ThreadSanitizer", cxxopts::value<bool>(options.tsan))

      ("debugmode", "Enable runtime debug checking (e.g. iterator validity checkers)", cxxopts::value<bool>(options.debugmode))
      ("debugsyms", "Enable generation of debug symbols", cxxopts::value<bool>(options.debugsyms))
      ("warnings", "Useful set of warnings", cxxopts::value<bool>(options.warnings))

      ("gcc", "Use GCC", cxxopts::value<bool>(options.gcc))
      ("libcxx", "Use libc++", cxxopts::value<bool>(options.libcxx))

      ("static", "Static", cxxopts::value<bool>(options.staticbuild))

      // TODO ("profile", "gprof-enabled build", cxxopts::value<bool>(options.profile))
      // TODO ("coverage", "gcov-enabled build", cxxopts::value<bool>(options.coverage))

      ("examples", "Build examples", cxxopts::value<bool>(options.examples))
      ("tests", "Build tests", cxxopts::value<bool>(options.tests))

      ("era", "Oldest compatible system. Example: winxp/win7/win10 (on windows), 10.14 (on macOS)", cxxopts::value<std::string>(options.target_era))

      ("help", "Print help")
  ;

  const auto result = args.parse(argc, argv);

  if (result.count("help")) {
    std::cout << args.help() << std::endl;
    return 0;
  }

  namespace fs = std::filesystem;
  fs::create_directory("build");
  fs::current_path("build");

  std::string config;
  std::string cmakeflags;
  std::string cflags;
  std::string lflags;

  // Common options
  cflags +=
      // Potentially makes the build faster
      " -pipe"

      // Allows to discard unused code more easily with --gc-sections
      " -ffunction-sections"
      " -fdata-sections"
  ;

  if constexpr(sys.os_windows)
  {
    // Remove obnoxious default <windows.h> features
    cflags +=
        " -DNOMINMAX" // min() / max() macros
        " -D_CRT_SECURE_NO_WARNINGS" // secure versions are not portable, don't use them
        " -DWIN32_LEAN_AND_MEAN" // makes including <windows.h> faster
    ;
  }

  if constexpr(!sys.os_apple)
  {
    lflags +=
        // LLD is a much faster linker : https://lld.llvm.org
        " -fuse-ld=lld"

        // We can tell lld to use threads
        " -Wl,--threads"

        // Makes debugging faster
        " -Wl,--gdb-index"

        // In conjunction with ffunction-sections / fdata-sections, removes unused code
        " -Wl,--gc-sections"

        // Use deep linking semantics on all platforms
        " -Bsymbolic"
        " -Bsymbolic-functions"
    ;
  }

  // General configuration flags
  if(options.fast)
  {
    if(options.debugsyms)
    {
      config = "RelWithDebInfo";
    }
    else
    {
      config = "Release";
      cflags += " -s ";
      lflags += " -g0 -s -Wl,-s ";
    }

    // By default enable LTO
    if(!options.thin_lto)
      options.full_lto = true;

    cflags += " -Ofast -march=native ";
    lflags += " -Ofast -march=native -Wl,--icf=safe -Wl,-O3 ";
  }
  else if(options.small)
  {
    config = "MinSizeRel";
    if(options.debugsyms)
    {
      cflags += " -g ";
    }
    else
    {
      cflags += " -s ";
      lflags += " -g0 -s -Wl,-s ";
    }
  }
  else if(options.debugsyms)
  {
    config = "Debug";
  }
  else
  {
    config = "RelWithDebInfo";
  }

  // Checks iterators and invariants in the stdlib and common libraries
  if(options.debugmode)
  {
    if(options.libcxx)
    {
      // See https://libcxx.llvm.org/docs/DesignDocs/DebugMode.html
      cflags +=
          " -D_LIBCPP_DEBUG=1 "
      ;
    }
    else
    {
      // See https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode.html
      cflags +=
          " -D_GLIBCXX_DEBUG=1 "
          " -D_GLIBCXX_DEBUG_PEDANTIC=1 "
      ;
    }
    // Note : Windows's stdlib has support for that too,
    // but we're mostly concerned with libc++
    // https://docs.microsoft.com/en-us/cpp/standard-library/iterator-debug-level

    // Boost.MultiIndex comes with similar abilities :
    // https://www.boost.org/doc/libs/1_72_0/libs/multi_index/doc/tutorial/debug.html
    cflags +=
        " -DBOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING=1 "
        " -DBOOST_MULTI_INDEX_ENABLE_SAFE_MODE=1 "
    ;
  }


  if(options.full_lto)
  {
    // Maximal optimization. Also needed for some optimizations to work,
    // e.g. -fvirtual-function-elimination
    cflags += " -flto=full -fvirtual-function-elimination ";
    lflags += " -flto=full -fvirtual-function-elimination ";
  }
  else if(options.thin_lto)
  {
    // https://clang.llvm.org/docs/ThinLTO.html
    // Builds faster but a tiny bit less performant
    cflags += " -flto=thin ";
    lflags += " -flto=thin ";
  }

  // Note that the ASAN and UBSAN cannot work in conjunction with TSAN.
  if(options.asan)
  {
    cflags += " -fsanitize=address -fno-omit-frame-pointer ";
    lflags += " -fsanitize=address -fno-omit-frame-pointer ";

    if(options.gcc)
    {
      // GCC does not have that by default, clang does
      cflags += "-D_GLIBCXX_SANITIZE_VECTOR";
    }
  }
  if(options.ubsan)
  {
    cflags += " -fsanitize=undefined -fsanitize=integer ";
    lflags += " -fsanitize=undefined -fsanitize=integer ";
  }

  if(!options.asan && !options.ubsan && options.tsan)
  {
    cflags += " -fsanitize=thread ";
    lflags += " -fsanitize=thread ";
  }

  if(options.staticbuild)
  {
    lflags += " -static-libgcc -static-libstdc++ -static ";
  }

  if(options.warnings)
  {
    cflags +=
      " -Wall"
      " -Wextra"
      " -pedantic"
      " -Wmisleading-indentation"
      " -Wnon-virtual-dtor"
      " -Wunused"
      " -Woverloaded-virtual"
      " -Werror=return-type"
      " -Werror=trigraphs"
      " -Wmissing-field-initializers"
      " -Wno-unused-parameter"
      " -Wno-unknown-pragmas"
      " -Wno-missing-braces"
      " -Wno-gnu-statement-expression"
      " -Wno-four-char-constants"
      " -Wno-cast-align"
      " -Wno-unused-local-typedef "
    ;

    if constexpr(sys.os_apple)
    {
      lflags +=
          " -Wl,-fatal_warnings"
          " -Wl,-undefined,dynamic_lookup "
      ;
    }
    else
    {
      lflags +=
          " -Wl,-z,defs"
          " -Wl,-z,now"
          " -Wl,--unresolved-symbols,report-all "
          " -Wl,--warn-unresolved-symbols "
          " -Wl,--no-undefined "
          " -Wl,--no-allow-shlib-undefined "
          " -Wl,--no-allow-multiple-definition "
      ;
    }
  }

  if(options.examples)
  {
    cmakeflags +=
        " -DBUILD_EXAMPLES=ON"
    ;
  }
  else
  {
    cmakeflags +=
        " -DBUILD_EXAMPLE=OFF"
        " -DBUILD_EXAMPLES=OFF"
    ;
  }
  if(options.tests)
  {
    cmakeflags +=
        " -DBUILD_TESTS=ON"
    ;
  }
  else
  {
    cmakeflags +=
        " -DWITH_TESTS=OFF"
        " -DBUILD_TEST=OFF"
        " -DBUILD_TESTS=OFF"
        " -DBUILD_TESTING=OFF"
    ;
  }

  if(!options.target_era.empty())
  {
    if constexpr(sys.os_apple)
    {
      cmakeflags += " -DCMAKE_OSX_DEPLOYMENT_TARGET=" + options.target_era;
    }
    else if constexpr(sys.os_windows)
    {
      cflags += " -D_WIN32_WINNT_=_WIN32_WINNT_" + uppercase(options.target_era);
    }
  }

  std::string cmd;
  cmd += "cmake"
         " .."
         " -G\"Ninja\" \\\n"
         " -DCMAKE_C_COMPILER=clang \\\n"
         " -DCMAKE_CXX_COMPILER=clang++ \\\n"

         " -DCMAKE_C_FLAGS=\"" + cflags + "\" \\\n"
         " -DCMAKE_CXX_FLAGS=\"" + cflags + "\" \\\n"
         " -DCMAKE_EXE_LINKER_FLAGS=\"" + lflags + "\" \\\n"
         " -DCMAKE_SHARED_LINKER_FLAGS=\"" + lflags + "\" \\\n"

         " -DCMAKE_INSTALL_PREFIX=install \\\n"

         " -DCMAKE_POSITION_INDEPENDENT_CODE=1 \\\n"
         " -DCMAKE_EXPORT_COMPILE_COMMANDS=1 \\\n"
         " -DCMAKE_CXX_STANDARD=20 \\\n"
         " -DCMAKE_SKIP_INSTALL_ALL_DEPENDENCY=1 \\\n"

         " -DCMAKE_C_VISIBILITY_PRESET=hidden \\\n"
         " -DCMAKE_CXX_VISIBILITY_PRESET=hidden \\\n"
         " -DCMAKE_VISIBILITY_INLINES_HIDDEN=1 \\\n"

         + cmakeflags;
  ;

  std::cout << "Configuring: \n$ " << cmd << std::endl;
  system(cmd.c_str());

  std::cout << "Building: \n$ cmake --build ." << std::endl;
  system("cmake --build .");
}
catch (const cxxopts::OptionException& e)
{
  std::cerr << e.what() << std::endl;

  return 1;
}
