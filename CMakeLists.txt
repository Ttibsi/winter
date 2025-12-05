cmake_minimum_required(VERSION 3.30)
project(winter LANGUAGES CXX)

option(RUN_TESTS "Run unit tests" OFF)

set(CMAKE_CXX_COMPILER "/usr/bin/clang++")
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

add_compile_options(-g)
add_compile_options(-Wall)
add_compile_options(-Wextra)
add_compile_options(-Wconversion)
add_compile_options(-Wimplicit-fallthrough)

add_subdirectory(src/winter)
add_executable(winter src/main.cpp)
target_link_libraries(winter PUBLIC winter-lang)

if(RUN_TESTS)
    message(STATUS "UNIT TESTS ENABLED")
    add_subdirectory(tests)
    add_executable(test_exe tests/main.cpp)
    target_link_libraries(test_exe PUBLIC winter-lang test-lib)
endif()

# if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
#     add_executable(willow_test src/main.cpp)
#     target_link_libraries(willow_test PRIVATE willow)
# endif()
