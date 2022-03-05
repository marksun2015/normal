# cmake version
cmake_minimum_required(VERSION 3.10)
# project name
project(Sample2)
#採用c++14標準
set(CMAKE_CXX_STANDARD 14)

include_directories(src)
add_subdirectory(src)
add_subdirectory(tst)
add_subdirectory(lib/googletest)
