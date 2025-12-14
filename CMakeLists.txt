cmake_minimum_required(VERSION 3.20)
project(rtree LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# --- Header-only library target (so tests & module can share include path cleanly)
add_library(rtree INTERFACE)
target_include_directories(rtree INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}/include)
target_compile_features(rtree INTERFACE cxx_std_17)

# --- Tests
enable_testing()
add_executable(test_houseData tests/test_houseData.cpp)
target_link_libraries(test_houseData PRIVATE rtree)
add_test(NAME houseData
         COMMAND test_houseData ${CMAKE_SOURCE_DIR}/Dataset/processed_house.csv)

add_executable(test_complex_fake tests/test_complex_fake.cpp)
target_link_libraries(test_complex_fake PRIVATE rtree)
add_test(NAME complex_fake COMMAND test_complex_fake)

add_executable(test_rect tests/test_rect.cpp)
target_link_libraries(test_rect PRIVATE rtree)
add_test(NAME rect COMMAND test_rect)

add_executable(test_insert tests/test_Insert.cpp)
target_link_libraries(test_insert PRIVATE rtree)
add_test(NAME insert COMMAND test_insert)

add_executable(test_delete tests/test_Delete.cpp)
target_link_libraries(test_delete PRIVATE rtree)
add_test(NAME delete COMMAND test_delete)

# --- Python / pybind11
find_package(Python REQUIRED COMPONENTS Interpreter Development.Module)

# Prefer system pybind11; fallback to FetchContent if not found
find_package(pybind11 CONFIG QUIET)
if(NOT pybind11_FOUND)
  include(FetchContent)
  FetchContent_Declare(
    pybind11
    GIT_REPOSITORY https://github.com/pybind/pybind11.git
    GIT_TAG v2.12.0
  )
  FetchContent_MakeAvailable(pybind11)
endif()

pybind11_add_module(rtree_bindings bind/mod.cpp)
target_link_libraries(rtree_bindings PRIVATE rtree)
set_target_properties(rtree_bindings PROPERTIES
  LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python
  RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/python
  ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/ignored
)


# cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
# cmake --build build -j
# ctest --test-dir build

# cmake -S . -B build_dbg -DCMAKE_BUILD_TYPE=Debug
# cmake --build build_dbg -j
# ctest --test-dir build_dbg
