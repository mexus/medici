cmake_minimum_required (VERSION 2.6)
project(approxx)
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Werror -Wall -Wextra ")

find_package (Threads)

add_subdirectory(test)
add_subdirectory(sources)
