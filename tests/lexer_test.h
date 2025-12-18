#include "winter/lexer.h"

using namespace std::string_view_literals;

constexpr int test_token_toString([[maybe_unused]] Willow::Test* test) {
    Winter::Token tok = Winter::Token(Winter::TokenType::NIL, 0, 3);
    auto str = tok.toString();

    if (str.size() != 38) {
        test->alert("String length: " + std::to_string(str.size()));
        return 1;
    }

    if (str.substr(11, 3) != "NIL") {
        test->alert("substr: " + str.substr(11, 3));
        return 2;
    }

    return 0;
}

constexpr int test_makeToken([[maybe_unused]] Willow::Test* test) {
    auto l = Winter::Lexer("");
    l.makeToken(Winter::TokenType::NIL, 0, 3);

    if (l.tokens.size() != 1) {
        return 1;
    }
    if (l.tokens.at(0).type != Winter::TokenType::NIL) {
        return 2;
    }
    if (l.tokens.at(0).start != 0) {
        return 3;
    }
    if (l.tokens.at(0).len != 3) {
        return 4;
    }
    return 0;
}

constexpr int test_scanNumber([[maybe_unused]] Willow::Test* test) {
    auto l = Winter::Lexer("42");

    if (l.scanNumber(0) != 2) {
        return 1;
    }
    if (l.tokens.at(0).type != Winter::TokenType::NUMBER) {
        return 2;
    }

    return 0;
}

constexpr int test_scanStringLiteral([[maybe_unused]] Willow::Test* test) {
    auto l = Winter::Lexer("\"hello world\"");

    if (l.scanStringLiteral(0) != 13) {
        return 1;
    }
    if (l.tokens.at(0).type != Winter::TokenType::STRING) {
        return 2;
    }
    return 0;
}

constexpr int test_validIdentChar([[maybe_unused]] Willow::Test* test) {
    const auto l = Winter::Lexer("");

    // return true
    if (!l.validIdentChar('_')) {
        return 1;
    }
    if (!l.validIdentChar('e')) {
        return 2;
    }
    if (!l.validIdentChar('J')) {
        return 3;
    }
    if (!l.validIdentChar('4')) {
        return 4;
    }

    // return false
    if (l.validIdentChar('!')) {
        return 5;
    }
    if (l.validIdentChar(':')) {
        return 6;
    }

    return 0;
}

constexpr int test_scanKeyword([[maybe_unused]] Willow::Test* test) {
    auto l = Winter::Lexer("nil");
    auto ret = l.scanKeyword("nil"sv, 0);
    if (!ret.has_value()) {
        return 1;
    }
    if (ret.value() != 3) {
        return 2;
    }

    ret = l.scanKeyword("nonsense"sv, 0);
    if (ret.has_value()) {
        return 3;
    }

    return 0;
}

constexpr int test_scanIdentifier([[maybe_unused]] Willow::Test* test) {
    auto l = Winter::Lexer("foo()");
    if (l.scanIdentifier(0) != 3) {
        return 1;
    }
    if (l.tokens.at(0).type != Winter::TokenType::IDENT) {
        return 2;
    }

    l = Winter::Lexer("returnValue");
    if (l.scanIdentifier(0) != 11) {
        return 3;
    }

    return 0;
}

constexpr int test_scanEllipsis([[maybe_unused]] Willow::Test* test) {
    return 1;
}

constexpr int test_tokenize([[maybe_unused]] Willow::Test* test) {
    struct Params {
        std::string text;
        std::size_t param_id;
        std::vector<Winter::TokenType> expected_tokens;
    };

    auto test_logic = [](const Params& params) -> std::size_t {
        auto l = Winter::Lexer(params.text);
        auto ret = l.tokenize();
        if (!ret.has_value()) {
            return params.param_id;
        }

        if (l.tokens.size() != params.expected_tokens.size()) {
            return params.param_id + 50;
        }

        for (std::size_t i = 0; i < params.expected_tokens.size(); i++) {
            if (l.tokens.at(i).type != params.expected_tokens.at(i)) {
                return i + params.param_id;
            }
        }

        return 0;
    };

    std::array<Params, 2> param_collection = {
        Params("", 100, {Winter::TokenType::END}),
        Params(
            "func f() { return 5; }", 200,
            {Winter::TokenType::FUNC, Winter::TokenType::IDENT, Winter::TokenType::LEFT_PAREN,
             Winter::TokenType::RIGHT_PAREN, Winter::TokenType::LEFT_BRACE,
             Winter::TokenType::RETURN, Winter::TokenType::NUMBER, Winter::TokenType::SEMICOLON,
             Winter::TokenType::RIGHT_BRACE, Winter::TokenType::END})};

    for (auto&& p : param_collection) {
        int val = int32_t(test_logic(p));
        if (val) {
            return val;
        }
    }

    return 0;
}
