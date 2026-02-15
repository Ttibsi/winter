#include <string>
#include <string_view>

#include <willow/willow.h>

#include "lexer.h"

using namespace std::literals::string_view_literals;

constexpr auto test_skipWhitespace([[maybe_unused]] Willow::Test* test) -> int {
    auto L = Winter::Lexer("   foo"sv, false);
    L.skipWhitespace();

    if (L.playhead != 3) {
        test->alert("Playhead = " + std::to_string(L.playhead));
        return 1;
    }

    return 0;
}

constexpr auto test_skipComment([[maybe_unused]] Willow::Test* test) -> int {
    std::string src = "# this is a comment\n  0";
    auto L = Winter::Lexer(src, false);
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

constexpr auto test_between([[maybe_unused]] Willow::Test* test) -> int {
    if (!Winter::between(1, 10, 5)) { return 1; }
    if (Winter::between(1, 10, 20)) { return 2; }
    if (!Winter::between(1, 10, 10)) { return 3; }

    return 0;
}

constexpr auto test_isNumeric([[maybe_unused]] Willow::Test* test) -> int {
    auto L = Winter::Lexer("0"sv, false);
    if (!L.isNumeric()) { return 1; }

    L.src = "a"sv;
    if (L.isNumeric()) { return 2; }

    L.src = "."sv;
    if (!L.isNumeric()) { return 3; }

    return 0;
}

constexpr auto test_isLetter([[maybe_unused]] Willow::Test* test) -> int {
    auto L = Winter::Lexer("0"sv, false);
    if (!L.isLetter()) { return 1; }

    L.src = "A"sv;
    if (!L.isLetter()) { return 2; }

    L.src = "a"sv;
    if (!L.isLetter()) { return 3; }

    L.src = " "sv;
    if (L.isLetter()) { return 4; }

    return 0;
}

constexpr auto test_lexNumeric([[maybe_unused]] Willow::Test* test) -> int {
    auto L = Winter::Lexer("012"sv, false);
    const auto result = L.lexNumeric();

    if (!result.has_value()) { return 1; }
    if (result.value().type != Winter::TokenType::NUM_LITERAL) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 3) {
        test->alert("length = " + std::to_string(result.value().len));
        return 4;
    }

    return 0;
}

constexpr auto test_lexSingle([[maybe_unused]] Willow::Test* test) -> int {
    auto L = Winter::Lexer(""sv, false);
    const auto result = L.lexSingle(Winter::TokenType::LPAREN);

    if (!result.has_value()) { return 1; }
    if (result.value().type != Winter::TokenType::LPAREN) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 1) { return 4; }
    if (L.playhead != 1) { return 5; }

    return 0;
}

constexpr auto test_lexDouble([[maybe_unused]] Willow::Test* test) -> int {
    auto L = Winter::Lexer("<="sv, false);
    const auto result = L.lexDouble('=', Winter::TokenType::LESS, Winter::TokenType::LESS_EQ);

    if (!result.has_value()) { return 1; }
    if (result.value().type != Winter::TokenType::LESS_EQ) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 2) { return 4; }
    if (L.playhead != 2) { return 5; }

    return 0;
}

constexpr auto test_lexChar([[maybe_unused]] Willow::Test* test) -> int {
    // Valid char
    auto L = Winter::Lexer("'v'"sv, false);
    const auto result = L.lexChar();

    if (!result.has_value()) { return 1; }
    if (result.value().type != Winter::TokenType::CHAR_LITERAL) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 3) { return 4; }
    if (L.playhead != 3) { return 5; }

    // Invalid (no closing quote)
    L.src = "'v"sv;
    const auto result2 = L.lexChar();
    if (result2.has_value()) { return 6; }

    return 0;
}

constexpr auto test_lexString([[maybe_unused]] Willow::Test* test) -> int {
    std::string src = "\"foo bar\"";
    auto L = Winter::Lexer(src, false);
    const auto result = L.lexString();

    if (!result.has_value()) { return 1; }
    if (result.value().type != Winter::TokenType::STR_LITERAL) { return 2; }
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

constexpr auto test_lexIdentKeyword([[maybe_unused]] Willow::Test* test) -> int {
    auto L = Winter::Lexer("", false);
    // Basic identifier
    L.src = "foo"sv;
    const auto result = L.lexIdentKeyword();

    if (!result.has_value()) { return 1; }
    if (result.value().type != Winter::TokenType::IDENT) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 3) { return 4; }
    if (L.playhead != 3) { return 5; }

    // keyword
    L.src = "type"sv;
    L.playhead = 0;
    const auto result2 = L.lexIdentKeyword();

    if (!result2.has_value()) { return 6; }
    if (result2.value().type != Winter::TokenType::TYPE) { return 7; }
    if (result2.value().start != 0) { return 8; }
    if (result2.value().len != 4) { return 9; }
    if (L.playhead != 4) { return 10; }

    return 0;
}

constexpr auto test_operator_funcCall([[maybe_unused]] Willow::Test* test) -> int {
    auto L = Winter::Lexer("let", false);
    const auto result = L();

    if (!result.has_value()) { return 1; }
    if (result.value().type != Winter::TokenType::LET) { return 2; }
    if (result.value().start != 0) { return 3; }
    if (result.value().len != 3) { return 4; }
    if (L.playhead != 3) { return 5; }

    return 0;
}
