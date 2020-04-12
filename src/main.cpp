#include <iostream>
#include <filesystem>
#include <cstdio>
#include <cxxopts.hpp>

// For dup
#if defined(_WIN32)
#include <io.h>
#else
#include <unistd.h>
#endif

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

bool check_command(const char* command)
{
  int res = system(command);
  std::cout << "\n";
  return res == 0;
}

bool check_environment()
{
  if(!check_command("clang -v")) {
    std::cout << "clang not found. Please install clang.\n";
    return false;
  }
  if(!check_command("cmake --version")) {
    std::cout << "cmake not found. Please install cmake.\n";
    return false;
  }
  if(!check_command("ninja --version")) {
    std::cout << "ninja not found. Please install ninja.\n";
    return false;
  }
#if (_WIN32)
  if(!check_command("echo 'int main(){}' | clang -x c++ - -o nul")) {
    std::cout << "lld not found. Please install lld.\n";
    return false;
  }
#else
  if(!check_command("echo 'int main(){}' | clang -x c++ - -o /dev/null")) {
    std::cout << "lld not found. Please install lld.\n";
    return false;
  }
#endif

  return true;
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

Options parse_options(int argc, char** argv)
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

      ("era", "Oldest compatible system. Example: era=winxp/win7/win10 (on windows), era=10.14 (on macOS)", cxxopts::value<std::string>(options.target_era))

      ("help", "Print help")
  ;

  const auto result = args.parse(argc, argv);

  if (result.count("help")) {
    std::cout << args.help() << std::endl;
    std::exit(0);
  }

  return options;
}

std::string generate_cmake_call(Options options)
{
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
    // Maximal optimization. Also needed for some optimizations to work.
    cflags += " -flto=full -fwhole-program-vtables ";
    lflags += " -flto=full -fwhole-program-vtables ";
    // Will only be available in clang-11
    // -fvirtual-function-elimination
    // -fvirtual-function-elimination
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

    // Check that all the functions we are calling do indeed exist
    // to prevent runtime crashes
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
         " -Wno-dev \\\n"
         " --no-warn-unused-cli \\\n"
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
         " -DCTEST_OUTPUT_ON_FAILURE=1 \\\n"

         + cmakeflags;
  ;
  return cmd;
}

std::string generate_build_path(Options options)
{
  std::string p = "build-";
  if(options.fast) p += "fast-";
  if(options.small) p += "small-";
  if(options.full_lto) p += "full-lto-";
  if(options.thin_lto) p += "thin-lto-";
  if(options.asan) p += "asan-";
  if(options.ubsan) p += "ubsan-";
  if(options.tsan) p += "tsan-";
  if(options.staticbuild) p += "static-";
  if(options.profile) p += "pgo-";
  if(options.coverage) p += "coverage-";
  if(options.debugsyms) p += "debugsyms-";
  if(options.debugmode) p += "debugmode-";
  if(options.warnings) p += "warnings-";
  if(options.gcc) p += "gcc-";
  if(options.libcxx) p += "libcxx-";
  if(options.examples) p += "examples-";
  if(options.tests) p += "tests-";
  if(!options.target_era.empty()) p += options.target_era + "-";

  // Remove last dash character
  p.pop_back();
  return p;
}
}

int main(int argc, char** argv) try
{
  // Sanity checks
  if(!check_environment())
    return 1;

  // Set-up
  const auto options = parse_options(argc, argv);

  const auto cmd = generate_cmake_call(options);
  const auto build_path = generate_build_path(options);

  // Create or go to build folder
  namespace fs = std::filesystem;
  {
    fs::create_directory(build_path);

    std::error_code ec;
    fs::current_path(build_path, ec);
    if(ec) {
      std::cout << "Could not cd into " << build_path << " ; aborting.";
      return 1;
    }
  }

  // Run cmake if necessary
  if (!fs::exists("build.ninja"))
  {
    std::cout << "Configuring: \n$ " << cmd << std::endl;
    if(int ret = system(cmd.c_str()); ret != 0) {
      return ret;
    }
  }

  if (!fs::exists("build.ninja"))
  {
    return 1;
  }

  return system("cmake --build .");
}
catch (const std::exception& e)
{
  std::cerr << e.what() << std::endl;
  return 1;
}
