cmake_minimum_required(VERSION 3.30)
project(winter LANGUAGES CXX)

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

# if(CMAKE_SOURCE_DIR STREQUAL CMAKE_CURRENT_SOURCE_DIR)
#     add_executable(willow_test src/main.cpp)
#     target_link_libraries(willow_test PRIVATE willow)
# endif()
