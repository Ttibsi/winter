function(build_tests)
    message(STATUS "UNIT TESTS ENABLED")
    include_directories(${PROJECT_SOURCE_DIR}/src)
    include(FetchContent)
    fetchcontent_declare(
        willow
        GIT_REPOSITORY https://github.com/Ttibsi/willow
        GIT_TAG        v0.0.1
    )
    fetchcontent_makeavailable(willow)

    add_executable(test_exe main.cpp)
    target_link_libraries(test_exe PUBLIC winter-lang willow)
endfunction()
