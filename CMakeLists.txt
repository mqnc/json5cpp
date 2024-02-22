cmake_minimum_required(VERSION 3.12)

project(json5cpp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -pedantic")

include_directories(src)

add_executable(test_suite test/main.cpp)

include(CTest)

add_test(NAME test_suite COMMAND test_suite)