#ifndef WINTER_LEXER_H
#define WINTER_LEXER_H

#include <cstdint>
#include <expected>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

#include "error.h"

using namespace std::literals::string_view_literals;

namespace Winter {
    enum class TokenType : uint8_t {
        // SYMBOLS
        LPAREN,
        RPAREN,
        LBRACE,
        RBRACE,
        LSQUACKET,
        RSQUACKET,
        COLON,
        SEMICOLON,
        PLUS,
        PLUS_PLUS,
        MINUS,
        MINUS_MINUS,
        STAR,
        SLASH,
        COMMA,
        DOT,
        DOT_DOT,

        // BOOLEAN OPERATORS
        GREATER,
        GREATER_EQ,
        LESS,
        LESS_EQ,
        EQUAL,
        EQUAL_EQ,
        NOT,
        NOT_EQ,

        // LOGICAL OPERATORS
        AND,
        OR,

        // KEYWORDS
        ALIAS,
        BREAK,
        CASE,
        CLASS,
        CONTINUE,
        DEFAULT,
        ENUM,
        FALSE,
        FOR,
        FUNC,
        IF,
        IMPLEMENTS,
        INTERFACE,
        LET,
        MOD,
        RETURN,
        STATIC,
        SWITCH,
        TRUE,
        TYPE,

        CHAR_LITERAL,
        NUM_LITERAL,
        STR_LITERAL,
        TYPE_LITERAL,
        IDENT,
        ERROR,
    };

    struct Token {
        TokenType type;
        std::size_t start;
        std::size_t len;

        explicit Token(TokenType t, std::size_t s) : type(t), start(s), len(0) {}
        explicit Token(TokenType t, std::size_t s, std::size_t l) : type(t), start(s), len(l) {}

        constexpr auto operator<=>(const Token&) const = default;
    };

    struct Lexer {
        std::vector<Token> tokens = {};
        std::size_t playhead = 0;
        std::string_view src;  // temp set in operator()

        const std::unordered_map<std::string_view, TokenType> keywords = {
            {"alias"sv, TokenType::ALIAS},
            {"break"sv, TokenType::BREAK},
            {"continue"sv, TokenType::CONTINUE},
            {"case"sv, TokenType::CASE},
            {"class"sv, TokenType::CLASS},
            {"default"sv, TokenType::DEFAULT},
            {"for"sv, TokenType::FOR},
            {"func"sv, TokenType::FUNC},
            {"if"sv, TokenType::IF},
            {"implements"sv, TokenType::IMPLEMENTS},
            {"interface"sv, TokenType::INTERFACE},
            {"let"sv, TokenType::LET},
            {"mod"sv, TokenType::MOD},
            {"return"sv, TokenType::RETURN},
            {"static"sv, TokenType::STATIC},
            {"switch"sv, TokenType::SWITCH},
            {"type"sv, TokenType::TYPE},
        };

        std::unordered_map<std::string_view, TokenType> types = {};

        explicit Lexer() {}
        auto skipWhitespace() -> void;
        [[nodiscard]] auto isNumeric() -> bool;
        [[nodiscard]] auto isLetter() -> bool;
        [[nodiscard]] auto lexNumeric() -> token_result_t;
        [[nodiscard]] auto lexSingle(const TokenType) -> token_result_t;
        [[nodiscard]] auto lexDouble(const char, const TokenType, const TokenType)
            -> token_result_t;
        [[nodiscard]] auto lexChar() -> token_result_t;
        [[nodiscard]] auto lexString() -> token_result_t;
        [[nodiscard]] auto lexIdentKeyword() -> token_result_t;
        [[nodiscard]] auto operator()(std::string_view src) -> token_result_t;
    };

    [[nodiscard]] auto between(int, int, int) -> bool;
}  // namespace Winter

#endif  // WINTER_LEXER_H
