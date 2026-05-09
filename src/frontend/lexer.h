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
        kw_else,
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

    struct Lexer;
    struct Token {
        TokenType type;
        std::size_t start;
        std::size_t len;

        explicit Token(TokenType t, std::size_t s) : type(t), start(s), len(0) {}
        explicit Token(TokenType t, std::size_t s, std::size_t l) : type(t), start(s), len(l) {}
        [[nodiscard]] constexpr static Token tombstone() { return Token(TokenType::error, 0); }

        [[nodiscard]] std::string toString(const Lexer* L) const noexcept;
        [[nodiscard]] int toNum(const Lexer* L) const noexcept;
        [[nodiscard]] char toChar(const Lexer* L) const noexcept;
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
            {"else"sv, TokenType::kw_else},
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
struct std::formatter<Winter::TokenType> {
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format specifier for TokenType");
        }
        return it;
    }

    auto format(Winter::TokenType tok, std::format_context& ctx) const {
        switch (tok) {
            case Winter::TokenType::lparen:      return std::format_to(ctx.out(), "lparen");
            case Winter::TokenType::rparen:      return std::format_to(ctx.out(), "rparen");
            case Winter::TokenType::lbrace:      return std::format_to(ctx.out(), "lbrace");
            case Winter::TokenType::rbrace:      return std::format_to(ctx.out(), "rbrace");
            case Winter::TokenType::lsquacket:   return std::format_to(ctx.out(), "lsquacket");
            case Winter::TokenType::rsquacket:   return std::format_to(ctx.out(), "rsquacket");
            case Winter::TokenType::colon:       return std::format_to(ctx.out(), "colon");
            case Winter::TokenType::semicolon:   return std::format_to(ctx.out(), "semicolon");
            case Winter::TokenType::star:        return std::format_to(ctx.out(), "star");
            case Winter::TokenType::slash:       return std::format_to(ctx.out(), "slash");
            case Winter::TokenType::comma:       return std::format_to(ctx.out(), "comma");
            case Winter::TokenType::plus:        return std::format_to(ctx.out(), "plus");
            case Winter::TokenType::minus:       return std::format_to(ctx.out(), "minus");
            case Winter::TokenType::dot:         return std::format_to(ctx.out(), "dot");
            case Winter::TokenType::plus_plus:   return std::format_to(ctx.out(), "plus_plus");
            case Winter::TokenType::minus_minus: return std::format_to(ctx.out(), "minus_minus");
            case Winter::TokenType::dot_dot:     return std::format_to(ctx.out(), "dot_dot");
            case Winter::TokenType::op_greater:  return std::format_to(ctx.out(), "op_greater");
            case Winter::TokenType::op_greater_eq:
                return std::format_to(ctx.out(), "op_greater_eq");
            case Winter::TokenType::op_less:     return std::format_to(ctx.out(), "op_less");
            case Winter::TokenType::op_less_eq:  return std::format_to(ctx.out(), "op_less_eq");
            case Winter::TokenType::op_equal:    return std::format_to(ctx.out(), "op_equal");
            case Winter::TokenType::op_equal_eq: return std::format_to(ctx.out(), "op_equal_eq");
            case Winter::TokenType::op_not:      return std::format_to(ctx.out(), "op_not");
            case Winter::TokenType::op_not_eq:   return std::format_to(ctx.out(), "op_not_eq");
            case Winter::TokenType::op_and:      return std::format_to(ctx.out(), "op_and");
            case Winter::TokenType::op_or:       return std::format_to(ctx.out(), "op_or");
            case Winter::TokenType::kw_alias:    return std::format_to(ctx.out(), "kw_alias");
            case Winter::TokenType::kw_break:    return std::format_to(ctx.out(), "kw_break");
            case Winter::TokenType::kw_continue: return std::format_to(ctx.out(), "kw_continue");
            case Winter::TokenType::kw_case:     return std::format_to(ctx.out(), "kw_case");
            case Winter::TokenType::kw_class:    return std::format_to(ctx.out(), "kw_class");
            case Winter::TokenType::kw_default:  return std::format_to(ctx.out(), "kw_default");
            case Winter::TokenType::kw_else:     return std::format_to(ctx.out(), "kw_else");
            case Winter::TokenType::kw_false:    return std::format_to(ctx.out(), "kw_false");
            case Winter::TokenType::kw_for:      return std::format_to(ctx.out(), "kw_for");
            case Winter::TokenType::kw_func:     return std::format_to(ctx.out(), "kw_func");
            case Winter::TokenType::kw_if:       return std::format_to(ctx.out(), "kw_if");
            case Winter::TokenType::kw_implements:
                return std::format_to(ctx.out(), "kw_implements");
            case Winter::TokenType::kw_interface: return std::format_to(ctx.out(), "kw_interface");
            case Winter::TokenType::kw_let:       return std::format_to(ctx.out(), "kw_let");
            case Winter::TokenType::kw_mod:       return std::format_to(ctx.out(), "kw_mod");
            case Winter::TokenType::kw_return:    return std::format_to(ctx.out(), "kw_return");
            case Winter::TokenType::kw_static:    return std::format_to(ctx.out(), "kw_static");
            case Winter::TokenType::kw_switch:    return std::format_to(ctx.out(), "kw_switch");
            case Winter::TokenType::kw_true:      return std::format_to(ctx.out(), "kw_true");
            case Winter::TokenType::kw_type:      return std::format_to(ctx.out(), "kw_type");
            case Winter::TokenType::num_literal:  return std::format_to(ctx.out(), "num_literal");
            case Winter::TokenType::char_literal: return std::format_to(ctx.out(), "char_literal");
            case Winter::TokenType::str_literal:  return std::format_to(ctx.out(), "str_literal");
            case Winter::TokenType::type_literal: return std::format_to(ctx.out(), "type_literal");
            case Winter::TokenType::ident:        return std::format_to(ctx.out(), "ident");
            case Winter::TokenType::eof:          return std::format_to(ctx.out(), "eof");
            case Winter::TokenType::error:        return std::format_to(ctx.out(), "error");
        }

        return std::format_to(ctx.out(), "");
    }
};

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
        return std::format_to(
            ctx.out(), "Type: {}, start: {}, len: {}", tok.type, tok.start, tok.len);
    }
};

#endif
