cmake_minimum_required(VERSION 3.30)
#se(CMAKE_CXX_COMPILER "/usr/bin/clang++")
project(winter LANGUAGES CXX)

option(RUN_TESTS "Run unit tests" OFF)

set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-g)
add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wconversion)
add_compile_options(-Wimplicit-fallthrough)

# repl
add_subdirectory(src/winter)
add_executable(winter src/main.cpp)
target_link_libraries(winter PUBLIC winter-lang)

# public-interface
add_executable(public src/main2.cpp)
target_link_libraries(winter PUBLIC winter-lang)

if(RUN_TESTS)
    message(STATUS "UNIT TESTS ENABLED")
    add_subdirectory(tests)
endif()
