#ifndef WINTER_PARSER_TEST_H
#define WINTER_PARSER_TEST_H

#include <willow/willow.h>

#include "frontend/parser.h"

using namespace Winter;

[[nodiscard]] constexpr int test_parser_check([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_consume_void([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_consume_tokens(
    [[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_parseBody([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_parseExpr([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_parseFunc([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_parseLet([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_parseNumLit([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_parseParam([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_parseReturn([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_parser_operator_call(
    [[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

#endif  // WINTER_PARSER_TEST_H
