#include "generate.hpp"

#include <cn/config.hpp>
#include <cn/fmt.hpp>
#include <cn/graph.hpp>
#include <cn/system.hpp>
#include <cn/util.hpp>

namespace cn
{

std::string generate_cmake_call(Options options)
{
  //  // General configuration flags
  //  if(options.fast)
  //  {
  //    if(options.debugsyms)
  //    {
  //      config = "RelWithDebInfo";
  //    }
  //    else
  //    {
  //      config = "Release";
  //
  //      // No debug info, strip symbols when linking
  //      lflags += " -g0 -s -Wl,-s ";
  //    }
  //
  //    // By default enable LTO
  //    if(!options.thin_lto)
  //      options.full_lto = true;
  //
  //    cflags += " -Ofast -march=native ";
  //    lflags += " -Ofast -march=native -Wl,--icf=all -Wl,--strip-all -Wl,-O3 ";
  //  }
  //  else if(options.small)
  //  {
  //    config = "MinSizeRel";
  //    if(options.debugsyms)
  //    {
  //      cflags += " -g ";
  //    }
  //    else
  //    {
  //      // No debug info, strip symbols when linking
  //      lflags += " -g0 -s -Wl,-s -Wl,--icf=all -Wl,--strip-all ";
  //    }
  //  }
  //  else if(options.debugsyms)
  //  {
  //    config = "RelWithDebInfo";
  //  }
  //  else
  //  {
  //    config = "Release";
  //  }
  //
  //  // Checks iterators and invariants in the stdlib and common libraries
  //  if(options.debugmode)
  //  {
  //    config = "Debug";
  //    if(options.libcxx)
  //    {
  //      // See https://libcxx.llvm.org/docs/DesignDocs/DebugMode.html
  //      cflags +=
  //          " -D_LIBCPP_DEBUG=1 "
  //      ;
  //    }
  //    else
  //    {
  //      // See https://gcc.gnu.org/onlinedocs/libstdc++/manual/debug_mode.html
  //      cflags +=
  //          " -D_GLIBCXX_DEBUG=1 "
  //          " -D_GLIBCXX_DEBUG_PEDANTIC=1 "
  //      ;
  //    }
  //    // Note : Windows's stdlib has support for that too,
  //    // but we're mostly concerned with libc++
  //    // https://docs.microsoft.com/en-us/cpp/standard-library/iterator-debug-level
  //
  //    // Boost.MultiIndex comes with similar abilities :
  //    // https://www.boost.org/doc/libs/1_72_0/libs/multi_index/doc/tutorial/debug.html
  //    cflags +=
  //        " -DBOOST_MULTI_INDEX_ENABLE_INVARIANT_CHECKING=1 "
  //        " -DBOOST_MULTI_INDEX_ENABLE_SAFE_MODE=1 "
  //    ;
  //  }

  //  if(options.warnings)
  //  {
  //    cflags +=
  //      " -Wall"
  //      " -Wextra"
  //      " -pedantic"
  //      " -Wmisleading-indentation"
  //      " -Wnon-virtual-dtor"
  //      " -Wunused"
  //      " -Woverloaded-virtual"
  //      " -Werror=return-type"
  //      " -Werror=trigraphs"
  //      " -Wmissing-field-initializers"
  //      " -Wno-unused-parameter"
  //      " -Wno-unknown-pragmas"
  //      " -Wno-missing-braces"
  //      " -Wno-gnu-statement-expression"
  //      " -Wno-four-char-constants"
  //      " -Wno-cast-align"
  //      " -Wno-unused-local-typedef "
  //    ;
  //
  //    // Check that all the functions we are calling do indeed exist
  //    // to prevent runtime crashes
  //    if constexpr(sys.os_apple)
  //    {
  //      lflags +=
  //          " -Wl,-fatal_warnings"
  //          " -Wl,-undefined,dynamic_lookup "
  //      ;
  //    }
  //    else
  //    {
  //      lflags +=
  //          " -Wl,-z,defs"
  //          " -Wl,-z,now"
  //          " -Wl,--unresolved-symbols,report-all "
  //          " -Wl,--warn-unresolved-symbols "
  //          " -Wl,--no-undefined "
  //          " -Wl,--no-allow-shlib-undefined "
  //          " -Wl,--no-allow-multiple-definition "
  //      ;
  //    }
  //  }

  std::string cmd;
  cmd += "cmake .. -GNinja -Wno-dev -DCMAKE_TOOLCHAIN_FILE=cninja-toolchain.cmake ";

  for (auto opt : options.cmake_options)
  {
    cmd += opt;
    cmd += ' ';
  }

  return cmd;
}

std::string generate_build_path(Options options)
{
  std::string p = "build-";
  auto options_names = options.options;
  std::sort(options_names.begin(), options_names.end());

  for (const auto& opt : options_names)
    p += fmt::format("{}-", opt);

  // Remove last dash character
  p.pop_back();
  return p;
}

std::string generate_toolchain(Options options)
{
  return graph{options.options}.generate();
}

}
