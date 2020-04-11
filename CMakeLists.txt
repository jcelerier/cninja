cmake_minimum_required(VERSION 3.10)
project(cninja LANGUAGES CXX)

add_executable(cninja)

target_include_directories(cninja
  PRIVATE
    3rdparty/cxxopts/include
)

target_sources(cninja
  PRIVATE
    src/main.cpp
)

target_compile_options(cninja
  PRIVATE
    $<IF:$<BOOL:${WIN32}>,/std:c++latest,-std=c++1z>
)
