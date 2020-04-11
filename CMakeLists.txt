cmake_minimum_required(VERSION 3.17)

project(cninja LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_executable(cninja)
target_sources(cninja
  PRIVATE
    src/main.cpp
)
target_include_directories(cninja
  PRIVATE
    3rdparty/cxxopts/include
)
