cmake_minimum_required(VERSION 3.20)
project(rtree LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(rtree INTERFACE)
target_include_directories(rtree INTERFACE include)

add_executable(test_rect tests/test_rect.cpp)
target_link_libraries(test_rect PRIVATE rtree)

include(CTest)
add_test(NAME rect COMMAND test_rect)
