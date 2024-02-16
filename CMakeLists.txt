cmake_minimum_required(VERSION 3.12)

project(json5cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

include_directories(src)

add_executable(test_parse test/parse.cpp)
add_executable(test_errors test/errors.cpp)
