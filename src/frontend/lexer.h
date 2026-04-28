#ifndef WINTER_LEXER_H
#define WINTER_LEXER_H

#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <string_view>
#include <unordered_map>

#include "../error.h"

namespace Winter {
    enum class TokenType : std::uint8_t {
        // symbols
        lparen,
        rparen,
        lbrace,
        rbrace,
        lsquacket,
        rsquacket,
        colon,
        semicolon,
        star,
        slash,
        comma,
        plus,
        minus,
        dot,
        plus_plus,
        minus_minus,
        dot_dot,

        // operators
        op_greater,
        op_greater_eq,
        op_less,
        op_less_eq,
        op_equal,
        op_equal_eq,
        op_not,
        op_not_eq,
        op_and,
        op_or,

        // keywords
        kw_alias,
        kw_break,
        kw_continue,
        kw_case,
        kw_class,
        kw_default,
        kw_false,
        kw_for,
        kw_func,
        kw_if,
        kw_implements,
        kw_interface,
        kw_let,
        kw_mod,
        kw_return,
        kw_static,
        kw_switch,
        kw_true,
        kw_type,

        // literals
        num_literal,
        char_literal,
        str_literal,
        type_literal,
        ident,

        eof,
        error
    };

    struct Token {
        TokenType type;
        std::size_t start;
        std::size_t len;

        explicit Token(TokenType t, std::size_t s) : type(t), start(s), len(0) {}
        explicit Token(TokenType t, std::size_t s, std::size_t l) : type(t), start(s), len(l) {}
        [[nodiscard]] constexpr static Token tombstone() { return Token(TokenType::error, 0); }
    };

    using namespace std::literals::string_view_literals;
    struct Lexer {
        std::size_t playhead;
        std::string_view src;
        const std::unordered_map<std::string_view, TokenType> keywords = {
            {"alias"sv, TokenType::kw_alias},
            {"break"sv, TokenType::kw_break},
            {"continue"sv, TokenType::kw_continue},
            {"case"sv, TokenType::kw_case},
            {"class"sv, TokenType::kw_class},
            {"default"sv, TokenType::kw_default},
            {"false"sv, TokenType::kw_false},
            {"for"sv, TokenType::kw_for},
            {"func"sv, TokenType::kw_func},
            {"if"sv, TokenType::kw_if},
            {"implements"sv, TokenType::kw_implements},
            {"interface"sv, TokenType::kw_interface},
            {"let"sv, TokenType::kw_let},
            {"mod"sv, TokenType::kw_mod},
            {"return"sv, TokenType::kw_return},
            {"static"sv, TokenType::kw_static},
            {"switch"sv, TokenType::kw_switch},
            {"true"sv, TokenType::kw_true},
            {"type"sv, TokenType::kw_type},
        };

        std::unordered_map<std::string_view, TokenType> types = {};

        explicit Lexer(std::string_view src) : playhead(0), src(src) {}
        void skipWhitespace() noexcept;
        void skipComment() noexcept;
        [[nodiscard]] bool isNumeric() noexcept;
        [[nodiscard]] bool isLetter() noexcept;
        [[nodiscard]] std::expected<Token, Error> lexSingle(TokenType) noexcept;
        [[nodiscard]] std::expected<Token, Error> lexDouble(char, TokenType, TokenType);
        [[nodiscard]] std::expected<Token, Error> lexChar();
        [[nodiscard]] std::expected<Token, Error> lexString();
        [[nodiscard]] std::expected<Token, Error> lexNumeric();
        [[nodiscard]] std::expected<Token, Error> lexIdentKeyword();
        [[nodiscard]] std::expected<Token, Error> operator()();
    };

    [[nodiscard]] bool between(int min, int max, int val) noexcept;
}  // namespace Winter

template <>
struct std::formatter<Winter::Token> {
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format specifier for Token");
        }
        return it;
    }

    auto format(Winter::Token tok, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "{}", "hi");
    }
};

#endif
