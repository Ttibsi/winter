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
        std::string tok_type = "";
        switch (tok.type) {
            case Winter::TokenType::lparen:        tok_type = "lparen"; break;
            case Winter::TokenType::rparen:        tok_type = "rparen"; break;
            case Winter::TokenType::lbrace:        tok_type = "lbrace"; break;
            case Winter::TokenType::rbrace:        tok_type = "rbrace"; break;
            case Winter::TokenType::lsquacket:     tok_type = "lsquacket"; break;
            case Winter::TokenType::rsquacket:     tok_type = "rsquacket"; break;
            case Winter::TokenType::colon:         tok_type = "colon"; break;
            case Winter::TokenType::semicolon:     tok_type = "semicolon"; break;
            case Winter::TokenType::star:          tok_type = "star"; break;
            case Winter::TokenType::slash:         tok_type = "slash"; break;
            case Winter::TokenType::comma:         tok_type = "comma"; break;
            case Winter::TokenType::plus:          tok_type = "plus"; break;
            case Winter::TokenType::minus:         tok_type = "minus"; break;
            case Winter::TokenType::dot:           tok_type = "dot"; break;
            case Winter::TokenType::plus_plus:     tok_type = "plus_plus"; break;
            case Winter::TokenType::minus_minus:   tok_type = "minus_minus"; break;
            case Winter::TokenType::dot_dot:       tok_type = "dot_dot"; break;
            case Winter::TokenType::op_greater:    tok_type = "op_greater"; break;
            case Winter::TokenType::op_greater_eq: tok_type = "op_greater_eq"; break;
            case Winter::TokenType::op_less:       tok_type = "op_less"; break;
            case Winter::TokenType::op_less_eq:    tok_type = "op_less_eq"; break;
            case Winter::TokenType::op_equal:      tok_type = "op_equal"; break;
            case Winter::TokenType::op_equal_eq:   tok_type = "op_equal_eq"; break;
            case Winter::TokenType::op_not:        tok_type = "op_not"; break;
            case Winter::TokenType::op_not_eq:     tok_type = "op_not_eq"; break;
            case Winter::TokenType::op_and:        tok_type = "op_and"; break;
            case Winter::TokenType::op_or:         tok_type = "op_or"; break;
            case Winter::TokenType::kw_alias:      tok_type = "kw_alias"; break;
            case Winter::TokenType::kw_break:      tok_type = "kw_break"; break;
            case Winter::TokenType::kw_continue:   tok_type = "kw_continue"; break;
            case Winter::TokenType::kw_case:       tok_type = "kw_case"; break;
            case Winter::TokenType::kw_class:      tok_type = "kw_class"; break;
            case Winter::TokenType::kw_default:    tok_type = "kw_default"; break;
            case Winter::TokenType::kw_false:      tok_type = "kw_false"; break;
            case Winter::TokenType::kw_for:        tok_type = "kw_for"; break;
            case Winter::TokenType::kw_func:       tok_type = "kw_func"; break;
            case Winter::TokenType::kw_if:         tok_type = "kw_if"; break;
            case Winter::TokenType::kw_implements: tok_type = "kw_implements"; break;
            case Winter::TokenType::kw_interface:  tok_type = "kw_interface"; break;
            case Winter::TokenType::kw_let:        tok_type = "kw_let"; break;
            case Winter::TokenType::kw_mod:        tok_type = "kw_mod"; break;
            case Winter::TokenType::kw_return:     tok_type = "kw_return"; break;
            case Winter::TokenType::kw_static:     tok_type = "kw_static"; break;
            case Winter::TokenType::kw_switch:     tok_type = "kw_switch"; break;
            case Winter::TokenType::kw_true:       tok_type = "kw_true"; break;
            case Winter::TokenType::kw_type:       tok_type = "kw_type"; break;
            case Winter::TokenType::num_literal:   tok_type = "num_literal"; break;
            case Winter::TokenType::char_literal:  tok_type = "char_literal"; break;
            case Winter::TokenType::str_literal:   tok_type = "str_literal"; break;
            case Winter::TokenType::type_literal:  tok_type = "type_literal"; break;
            case Winter::TokenType::ident:         tok_type = "ident"; break;
            case Winter::TokenType::eof:           tok_type = "eof"; break;
            case Winter::TokenType::error:         tok_type = "error"; break;
        }

        return std::format_to(
            ctx.out(), "Type: {}, start: {}, len: {}", tok_type, tok.start, tok.len);
    }
};

#endif
