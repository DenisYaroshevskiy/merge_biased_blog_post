cmake_minimum_required(VERSION 3.7)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(merge_biased_blog_post)

include_directories(./)

add_compile_options(-Wall -Wextra -Werror -Wpedantic -g)

add_executable(test)
target_sources(test PRIVATE
               other_algorithms_test.cc
               third_party/main_catch.cc)
set_property(TARGET test PROPERTY CXX_STANDARD 14)
target_compile_options(test PRIVATE -O1 -fno-omit-frame-pointer -g -fsanitize=address)
target_link_libraries(test PRIVATE -fsanitize=address)

add_executable(benchmarks)
target_sources(benchmarks PRIVATE
               merge_benchmark.cc
               third_party/google_benchmark_main.cc)
set_property(TARGET benchmarks PROPERTY CXX_STANDARD 17)
target_compile_options(benchmarks PRIVATE -O3)
target_link_libraries(benchmarks benchmark)