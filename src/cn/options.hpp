#pragma once
#include <cn/fmt.hpp>
#include <cxxopts.hpp>

#if defined(_WIN32)
#undef small // seriously win32...
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

static
Options parse_options(int argc, char** argv)
{
  Options options{};

  cxxopts::Options args{"cninja", "cninja - an opinionated cmake frontend"};
      args.show_positional_help()
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
    fmt::print("{}\n", args.help());
    std::exit(0);
  }

  return options;
}
}
