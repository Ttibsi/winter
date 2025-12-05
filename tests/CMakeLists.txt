include(FetchContent)
fetchcontent_declare(
    willow
    GIT_REPOSITORY https://github.com;ttibsi/willow
    GIT_TAG        v0.0.1
)
fetchcontent_makeavailable(willow)

add_library(test-lib STATIC)
set_target_properties(test-lib PROPERTIES LINKER_LANGUAGE CXX)

target_sources(test-lib PUBLIC
)
target_link_libraries(test-lib PRIVATE willow)
