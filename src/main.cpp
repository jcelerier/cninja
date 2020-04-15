#include <cn/options.hpp>
#include <cn/check.hpp>
#include <cn/generate.hpp>

int main(int argc, char** argv) try
{
  using namespace cn;

  // Set-up
  const auto options = parse_options(argc, argv);

  // Sanity checks
  if(!check_environment())
    return 1;

  // If we are in a build dir we just run a build
  if(!fs::exists("build.ninja"))
  {
    const auto cmd = generate_cmake_call(options);
    const auto build_path = generate_build_path(options);

    // Create or go to build folder
    {
      fs::create_directory(build_path);

      std::error_code ec;
      fs::current_path(build_path, ec);
      if(ec) {
        fmt::print("Could not cd into {} ; aborting.\n", build_path);
        return 1;
      }
    }

    // Run cmake if necessary
    if (!fs::exists("build.ninja"))
    {
      fmt::print("Configuring: \n$ {}\n", cmd);
      if(int ret = system(cmd.c_str()); ret != 0) {
        return ret;
      }
    }
  }

  // Run the build
  return system("cmake --build .");
}
catch (const std::exception& e)
{
  fmt::print("Error: {}\nRun \"cninja help\" for help.", e.what());
  return 1;
}
