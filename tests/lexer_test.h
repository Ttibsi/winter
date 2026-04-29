#ifndef WINTER_LEXER_TEST_H
#define WINTER_LEXER_TEST_H

#include <string>
#include <string_view>

#include <willow/willow.h>

#include "frontend/lexer.h"

using namespace Winter;
using namespace std::literals::string_view_literals;

[[nodiscard]] constexpr int test_between([[maybe_unused]] Willow::Test* test) noexcept {
    if (!between(1, 10, 5)) { return 1; }
    if (between(1, 10, 20)) { return 2; }
    if (!between(1, 10, 10)) { return 3; }

    return 0;
}

[[nodiscard]] constexpr int test_skipWhitespace([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("   foo"sv);
    L.skipWhitespace();

    if (L.playhead != 3) {
        test->alert("Playhead = " + std::to_string(L.playhead));
        return 1;
    }

    return 0;
}

[[nodiscard]] constexpr int test_skipComment([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("# this is a comment\n  0"sv);
    L.skipComment();

    if (L.playhead != 19) {
        test->alert("Playhead = " + std::to_string(L.playhead));
        return 1;
    }

    L.skipWhitespace();
    if (L.playhead != 22) {
        test->alert("Playhead = " + std::to_string(L.playhead));
        return 2;
    }

    return 0;
}

[[nodiscard]] constexpr int test_isNumeric([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("0"sv);
    if (!L.isNumeric()) { return 1; }

    L.src = "a"sv;
    if (L.isNumeric()) { return 2; }

    L.src = "."sv;
    if (!L.isNumeric()) { return 3; }

    return 0;
}

[[nodiscard]] constexpr int test_isLetter([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("0"sv);
    if (!L.isLetter()) { return 1; }

    L.src = "A"sv;
    if (!L.isLetter()) { return 2; }

    L.src = "a"sv;
    if (!L.isLetter()) { return 3; }

    L.src = " "sv;
    if (L.isLetter()) { return 4; }

    return 0;
}

[[nodiscard]] constexpr int test_lexSingle([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("(");
    const auto result = L.lexSingle(TokenType::lparen);

    if (!result.has_value()) { return 1; }
    if (result.value().type != TokenType::lparen) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 1) { return 4; }
    if (L.playhead != 1) { return 5; }

    return 0;
}

[[nodiscard]] constexpr int test_lexDouble([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("<="sv);
    const auto result = L.lexDouble('=', TokenType::op_less, TokenType::op_less_eq);

    if (!result.has_value()) { return 1; }
    if (result.value().type != TokenType::op_less_eq) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 2) { return 4; }
    if (L.playhead != 2) { return 5; }

    return 0;
}

[[nodiscard]] constexpr int test_lexChar([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("'v'"sv);
    // Valid char
    const auto result = L.lexChar();

    if (!result.has_value()) { return 1; }
    if (result.value().type != TokenType::char_literal) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 3) { return 4; }
    if (L.playhead != 3) { return 5; }

    // Invalid (no closing quote)
    L.src = "'v"sv;
    const auto result2 = L.lexChar();
    if (result2.has_value()) { return 6; }

    return 0;
}

[[nodiscard]] constexpr int test_lexString([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("\"foo bar\""sv);
    const auto result = L.lexString();

    if (!result.has_value()) { return 1; }
    if (result.value().type != TokenType::str_literal) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 9) {
        test->alert("String length: " + std::to_string(result.value().len));
        return 4;
    }
    if (L.playhead != 9) { return 5; }

    // Invalid (no closing quote)
    L.src = "\"foo bar"sv;
    const auto result2 = L.lexChar();
    if (result2.has_value()) { return 6; }

    return 0;
}

[[nodiscard]] constexpr int test_lexNumeric([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("012"sv);
    const auto result = L.lexNumeric();

    if (!result.has_value()) { return 1; }
    if (result.value().type != TokenType::num_literal) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 3) {
        test->alert("length = " + std::to_string(result.value().len));
        return 4;
    }

    return 0;
}

[[nodiscard]] constexpr int test_lexIdentKeyword([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("foo"sv);
    // Basic identifier
    const auto result = L.lexIdentKeyword();

    if (!result.has_value()) { return 1; }
    if (result.value().type != TokenType::ident) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 3) { return 4; }
    if (L.playhead != 3) { return 5; }

    // keyword
    L.src = "type"sv;
    L.playhead = 0;
    const auto result2 = L.lexIdentKeyword();

    if (!result2.has_value()) { return 6; }
    if (result2.value().type != TokenType::kw_type) { return 7; }
    if (result2.value().start != 0) { return 8; }
    if (result2.value().len != 4) { return 9; }
    if (L.playhead != 4) { return 10; }

    return 0;
}

[[nodiscard]] constexpr int test_operator_funcCall([[maybe_unused]] Willow::Test* test) noexcept {
    auto L = Lexer("let");
    const auto result = L();

    if (!result.has_value()) { return 1; }
    if (result.value().type != TokenType::kw_let) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 3) { return 4; }
    if (L.playhead != 3) { return 5; }

    return 0;
}

#endif
