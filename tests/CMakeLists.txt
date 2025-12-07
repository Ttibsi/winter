include_directories(${PROJECT_SOURCE_DIR}/src/winter)

include(FetchContent)
fetchcontent_declare(
    willow
    GIT_REPOSITORY https://github.com/Ttibsi/willow
    GIT_TAG        ccc88b8753a74b6ab9b66e67e81c2169780b4eed
)
fetchcontent_makeavailable(willow)

add_executable(test_exe ${CMAKE_CURRENT_SOURCE_DIR}/main.cpp)
target_include_directories(test_exe PRIVATE ${PROJECT_SOURCE_DIR}/src/winter)
target_link_libraries(test_exe PUBLIC winter-lang willow)
