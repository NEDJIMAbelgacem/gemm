cmake_minimum_required(VERSION 3.0.0)
project(halide_challenge VERSION 0.1.0)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)

find_package(Halide REQUIRED)

add_executable(halide_implementation halide_implementation.cpp)
target_link_libraries(halide_implementation PRIVATE Halide::Halide Halide::ImageIO Halide::Tools)

add_executable(c_implementation c_implementation.c)
target_link_libraries(c_implementation)

add_executable(compare_performance compare_performance.cpp)
target_link_libraries(compare_performance)

add_executable(compare_results compare_results.cpp)
target_link_libraries(compare_results)