# cninja - an opinionated cmake config manager
![Build status](https://github.com/jcelerier/cninja/workflows/Build/badge.svg)

A project trying to set 22th century defaults to CMake calls, with a simplified syntax for the most common needs during development and an emphasis on build speed.

The tool works by generating a CMake toolchain file from a list of features passed in arguments, and running a `cmake ; cmake --build` cycle with that toolchain file. Requested features can have (acyclic) dependencies and will be ordered automatically if they do.

## Installing
Either download the [latest binary for your OS](https://github.com/jcelerier/cninja/releases/latest) or install it through your package manager of choice:  
- ArchLinux AUR: `yay -S cninja`
- MSYS2: `pacman -S cninja`
- Homebrew: `brew tap jcelerier/packages && brew install cninja`

## Features

- Uses Ninja, Clang everywhere, and if possible libc++ and lld.
- Generates `compile_commands.json` by default.
- Forces you to use a recent clang with a recent C++ standard.
- Configures and builds in one glorious stroke.
- Does not scream at you.
- Does not try to install in global system folders - the `install` target will copy things in a `build/install` folder.
- Looks for config files in `.cninja` folders - easily create custom configs for your projects. 

## Usage

    ### Produces a release build with debug symbols in "build"
    $ git clone something
    $ cd something
    $ cninja

    ### Produces a build in "build-static-lto=thin"
    $ cninja static lto=thin

    ### Produces a build in "build-asan-ubsan-debugsyms-examples-tests"
    $ cninja asan ubsan debugsyms tests examples
    
    ### Produces a build of the /foo/bar folder with some CMake flags
    $ cninja /foo/bar asan ubsan debugsyms -- -DCMAKE_PREFIX_PATH=/path/to/somelib
    
    ### Custom configurations 
    ### Can be per-project: 
    $ mkdir .cninja
    $ echo 'string(APPEND CMAKE_CXX_FLAGS_INIT " -Weverything")' > .cninja/all_warnings.cmake
    
    ### Or per-user:
    $ mkdir $HOME/.cninja
    $ echo 'set(CMAKE_PREFIX_PATH "/home/myself/Qt/5.15/gcc_64")' > $HOME/.cninja/my_qt.cmake
    
    $ cninja all_warnings my_qt asan
    
    ### Don't set any of the default flags (i.e. use CMake defaults)
    $ cninja plain
    

As an example, here is a comparison of the default set-up of `cninja` and the same build type for CMake on the [Strawberry](https://github.com/strawberrymusicplayer/strawberry) software : 

| Tool   | Command                                                                 | First build | After changing a file |
|--------|-------------------------------------------------------------------------|-------------|-----------------------|
| cmake  | mkdir build<br> cd build<br> cmake -DCMAKE_BUILD_TYPE=RelWithDebInfo ..<br>cmake --build . -- -j16   | 2m 40s      | 7.6s                  |
| cninja | cninja                                                                  |  2m 16s     | 1.7s                  |


## Goals

The main goal is to uniformize semantics across platforms, with the strictest choice prevailing, to make portability easier and not have bad surprises when porting software written on Linux to macOS or Windows.

For instance :
 * Symbols are hidden by default.
 * Dynamic library symbols are resolved at compile-time and early-bound.
 * Debug mode will perform iterator checking on all platforms.

It also splits the common "Debug" feature in its two orthogonal child features :
 * `debugsyms` will generate binaries with debug symbols.
 * `debugmode` will enable run-time checks and assertions in the standard library - the default for Debug in Windows, but generally not in Linux and Mac platforms.

A further goal is to remove the need for adding various flags in each CMake file - ideally, CMake build scripts should just create targets and link libraries, not set up compiler flags. 
Running for instance 

    $ rg --no-filename --only-matching '[A-Za-z0-9_]+_SANITIZE[A-Z]?' -g '*.cmake' | sort | uniq

in my home directory reveals almost 30 distinct variables used in various CMake scripts to enable sanitizers... and even more if I also match CMakeLists.txt files. 

![No More !](https://66.media.tumblr.com/1e058c57c5907d24aa2a8508fe533fdc/tumblr_mx3vrl8ge41qjoke8o2_250.gif)

# "Is cninja for me ?"

- I downloaded a CMake project on Github and want to build it *presto*

  🡆 Yes

- I have wrist pain from the mere thought of typing `CMAKE_CXX_FLAGS_RELWITHDEBINFO` once more

  🡆 Yes

- My CMake files have options to enable sanitizers, LTO and build warnings

  🡆 Yes (also, clean them !)

- My projects have bash / powershell / python / perl / makefile / .travis.yml scripts which invoke CMake

  🡆 Yes
  
- `history | grep cmake | grep CMAKE_` in my shell fills my terminal screen

  🡆 Yes

- I want to get shit done

  🡆 Yes

- I want to cross-compile

  🡆 No

- My project only builds in C++98 mode

  🡆 No

- I don't want my code to be portable across platforms

  🡆 No

- I like typing CFLAGS="..." CXXFLAGS="..."

  🡆 You lie

- I want Haiku or SerenityOS support

  🡆 PRs accepted

# Roadmap

- PGO
- Code coverage
- ~Debug split -> done
- Downloading Clang / CMake / Ninja automatically if they aren't available
- Some kind of improved support for common libraries
    - Looking for Qt in `~/Qt/system/latest_version`
    - Looking for Qt in `c:/Qt/system/latest_version`

# Thanks

Cristian Adam for his [blog post on using Github actions](https://cristianadam.eu/20191222/using-github-actions-with-c-plus-plus-and-cmake/).
