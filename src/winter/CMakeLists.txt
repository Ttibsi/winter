add_library(winter-lang STATIC)
set_target_properties(winter-lang PROPERTIES LINKER_LANGUAGE CXX)

target_sources(winter-lang PUBLIC
    bytecode.cpp
    helpers.cpp
    lexer.cpp
    parser.cpp
)

target_include_directories(winter-lang PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/..)
