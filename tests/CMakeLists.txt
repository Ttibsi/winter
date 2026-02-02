include_directories(${PROJECT_SOURCE_DIR}/src)

add_compile_options(-g)

include(FetchContent)
fetchcontent_declare(
    willow
    GIT_REPOSITORY https://github.com/Ttibsi/willow
    GIT_TAG        main
)
fetchcontent_makeavailable(willow)

add_executable(test_exe ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_include_directories(test_exe PRIVATE ${PROJECT_SOURCE_DIR}/src/winter)
target_link_libraries(test_exe PUBLIC winter_src willow)
