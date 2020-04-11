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
std::string toupper(std::string s) noexcept
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

  bool asserts{};
  bool warnings{};
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
      ("fast", "Creates an optimized build", cxxopts::value<bool>(options.fast))
      ("small", "Creates a small build", cxxopts::value<bool>(options.small))

      ("lto", "LTO", cxxopts::value<bool>(options.full_lto))
      ("full-lto", "LTO", cxxopts::value<bool>(options.full_lto))
      ("thin-lto", "Thin LTO", cxxopts::value<bool>(options.thin_lto))

      ("asan", "AddressSanitizer", cxxopts::value<bool>(options.asan))
      ("ubsan", "UndefinedBehaviourSanitizer", cxxopts::value<bool>(options.ubsan))
      ("tsan", "ThreadSanitizer", cxxopts::value<bool>(options.tsan))

      ("asserts", "Enable common run-time debug options", cxxopts::value<bool>(options.asserts))
      ("warnings", "Useful set of warnings", cxxopts::value<bool>(options.warnings))

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
    exit(0);
  }

  namespace fs = std::filesystem;
  fs::create_directory("build");
  fs::current_path("build");

  std::string cmakeflags;
  std::string cflags;
  std::string lflags;
  std::string release_cflags;
  std::string release_lflags;

  // Common options
  cflags +=
      " -pipe"
      " -ffunction-sections"
      " -fdata-sections"
  ;

  if constexpr(sys.os_windows)
  {
    cflags +=
        " -DNOMINMAX"
        " -D_CRT_SECURE_NO_WARNINGS"
        " -DWIN32_LEAN_AND_MEAN"
    ;
  }

  if constexpr(!sys.os_apple)
  {
    lflags +=
        " -fuse-ld=lld"
        " -Wl,--threads"
        " -Wl,--gdb-index"
        " -Wl,--gc-sections"
        " -Bsymbolic"
        " -Bsymbolic-functions "
    ;
  }

  // Specific options
  if(options.fast)
  {
    release_cflags += " -Ofast -march=native ";
    release_lflags += " -Ofast -march=native -Wl,--icf=safe -Wl,-O3 ";
  }

  if(options.small)
  {
    cflags += " -s ";
    lflags += " -g0 -s -Wl,-s ";
    release_cflags += " -Os ";
    release_lflags += " -Os ";
  }

  if(options.full_lto)
  {
    release_cflags += " -flto=full ";
    release_lflags += " -flto=full ";
  }
  else if(options.thin_lto)
  {
    release_cflags += " -flto=thin ";
    release_lflags += " -flto=thin ";
  }

  if(options.asan)
  {
    release_cflags += " -fsanitize=address -fno-omit-frame-pointer ";
    release_lflags += " -fsanitize=address -fno-omit-frame-pointer ";
  }
  else if(options.ubsan)
  {
    release_cflags += " -fsanitize=undefined -fsanitize=integer ";
    release_lflags += " -fsanitize=undefined -fsanitize=integer ";
  }
  else if(options.tsan)
  {
    release_cflags += " -fsanitize=thread ";
    release_lflags += " -fsanitize=thread ";
  }

  if(options.staticbuild)
  {
    lflags += " -static-libgcc -static-libstdc++ -static ";
  }

  if(options.asserts)
  {
    if(options.libcxx)
    {
      cflags +=
          " -D_LIBCPP_DEBUG=1 "
      ;
    }
    else
    {
      cflags +=
          " -D_GLIBCXX_DEBUG=1 "
          " -D_GLIBCXX_DEBUG_PEDANTIC=1 "
      ;
    }

    cflags +=
        " -DBOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING=1 "
        " -DBOOST_MULTI_INDEX_ENABLE_SAFE_MODE=1 "
    ;
  }

  if(options.warnings)
  {
    cflags +=
      " -Wall"
      " -Wextra"
      " -Wmisleading-indentation"
      " -Wno-unused-parameter"
      " -Wno-unknown-pragmas"
      " -Wno-missing-braces"
      " -Wnon-virtual-dtor"
      " -pedantic"
      " -Wunused"
      " -Woverloaded-virtual"
      " -Werror=return-type"
      " -Werror=trigraphs"
      " -Wmissing-field-initializers"

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
      cflags += " -D_WIN32_WINNT_=_WIN32_WINNT_" + toupper(options.target_era);
    }
  }

  std::string cmd;
  cmd += "cmake"
         " .."
         " -G\"Ninja Multi-Config\" \\\n"
         " -DCMAKE_C_COMPILER=clang \\\n"
         " -DCMAKE_CXX_COMPILER=clang++ \\\n"

         " -DCMAKE_C_FLAGS=\"" + cflags + "\" \\\n"
         " -DCMAKE_CXX_FLAGS=\"" + cflags + "\" \\\n"
         " -DCMAKE_EXE_LINKER_FLAGS=\"" + lflags + "\" \\\n"
         " -DCMAKE_SHARED_LINKER_FLAGS=\"" + lflags + "\" \\\n"

         " -DCMAKE_C_FLAGS_RELEASE=\"" + release_cflags + "\" \\\n"
         " -DCMAKE_CXX_FLAGS_RELEASE=\"" + release_cflags + "\" \\\n"
         " -DCMAKE_EXE_LINKER_FLAGS_RELEASE=\"" + release_lflags + "\" \\\n"
         " -DCMAKE_SHARED_LINKER_FLAGS_RELEASE=\"" + release_lflags + "\" \\\n"

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
  exit(1);
}
