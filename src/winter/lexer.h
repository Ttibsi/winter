#ifndef WINTER_LEXER_H
#define WINTER_LEXER_H

#include <array>
#include <expected>
#include <optional>
#include <regex>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

#include "error.h"

namespace Winter {
    enum class TokenType {
        // Single-character tokens.
        LEFT_PAREN,
        RIGHT_PAREN,
        LEFT_BRACE,
        RIGHT_BRACE,
        COMMA,
        DOT,
        SEMICOLON,
        COLON,
        MINUS,
        PLUS,
        STAR,
        SLASH,

        // One or two character tokens.
        BANG,
        BANG_EQ,
        EQUAL,
        EQUAL_EQ,
        GREATER,
        GREATER_EQ,
        LESS,
        LESS_EQ,
        ELLIPSIS,

        // digraphs
        AND,
        OR,

        // Keywords.
        CASE,
        CLASS,
        CONST,
        DEFAULT,
        ELSE,
        ENUM,
        EXPORT,
        FALSE,
        FOR,
        FUNC,
        IF,
        IMPORT,
        NIL,
        RETURN,
        STATIC,
        SWITCH,
        TRUE,
        VAR,

        // Literals.
        IDENT,
        STRING,
        CHAR,
        NUMBER,

        END  // End Of File -- EOF is a macro
    };

    struct Token {
        TokenType type;
        std::size_t start;
        std::size_t len;

        explicit Token(TokenType typ, std::size_t s, std::size_t l) : type(typ), start(s), len(l) {}

        auto operator<=>(const Token&) const = default;

        [[nodiscard]] std::string toString() const;
    };

    struct RegexPair {
        std::regex regex;
        TokenType type;
        std::size_t length;
        explicit constexpr RegexPair(std::regex r, TokenType tok, std::size_t sz)
            : regex(r), type(tok), length(sz) {}
    };

    struct Lexer {
        std::string raw_text;
        std::vector<Token> tokens;
        const std::array<RegexPair, 18> keyword_regex = {
            RegexPair(std::regex("^(case)(?![a-zA-Z0-9_])"), TokenType::CASE, 4),
            RegexPair(std::regex("^(class)(?![a-zA-Z0-9_])"), TokenType::CLASS, 5),
            RegexPair(std::regex("^(const)(?![a-zA-Z0-9_])"), TokenType::CONST, 5),
            RegexPair(std::regex("^(default)(?![a-zA-Z0-9_])"), TokenType::DEFAULT, 7),
            RegexPair(std::regex("^(else)(?![a-zA-Z0-9_])"), TokenType::ELSE, 4),
            RegexPair(std::regex("^(enum)(?![a-zA-Z0-9_])"), TokenType::ENUM, 4),
            RegexPair(std::regex("^(export)(?![a-zA-Z0-9_])"), TokenType::EXPORT, 6),
            RegexPair(std::regex("^(false)(?![a-zA-Z0-9_])"), TokenType::FALSE, 5),
            RegexPair(std::regex("^(for)(?![a-zA-Z0-9_])"), TokenType::FOR, 3),
            RegexPair(std::regex("^(func)(?![a-zA-Z0-9_])"), TokenType::FUNC, 4),
            RegexPair(std::regex("^(if)(?![a-zA-Z0-9_])"), TokenType::IF, 2),
            RegexPair(std::regex("^(import)(?![a-zA-Z0-9_])"), TokenType::IMPORT, 6),
            RegexPair(std::regex("^(nil)(?![a-zA-Z0-9_])"), TokenType::NIL, 3),
            RegexPair(std::regex("^(return)(?![a-zA-Z0-9_])"), TokenType::RETURN, 6),
            RegexPair(std::regex("^(static)(?![a-zA-Z0-9_])"), TokenType::STATIC, 6),
            RegexPair(std::regex("^(switch)(?![a-zA-Z0-9_])"), TokenType::SWITCH, 6),
            RegexPair(std::regex("^(true)(?![a-zA-Z0-9_])"), TokenType::TRUE, 4),
            RegexPair(std::regex("^(var)(?![a-zA-Z0-9_])"), TokenType::VAR, 3),
        };

        std::size_t playhead = 0;

        explicit Lexer(std::string src) : raw_text(src), tokens({}) {}

        // move assignment op
        Lexer& operator=(Lexer&& other) noexcept {
            if (this != &other) {
                raw_text = std::move(other.raw_text);
                tokens = std::move(other.tokens);
            }
            return *this;
        }

        void makeToken(TokenType, long, std::size_t);
        [[nodiscard]] std::size_t scanNumber(std::size_t);
        [[nodiscard]] std::size_t scanStringLiteral(std::size_t);
        [[nodiscard]] std::size_t scanChar(std::size_t);
        [[nodiscard]] bool validIdentChar(char) const;
        [[nodiscard]] std::optional<std::size_t> scanKeyword(std::string_view, std::size_t);
        [[nodiscard]] std::size_t scanIdentifier(std::size_t);
        [[nodiscard]] std::size_t scanEllipsis(std::size_t);
        [[nodiscard]] std::expected<void, Err> tokenize();

        [[nodiscard]] std::expected<void, Err> advance(const TokenType&);
        void advance();
        [[nodiscard]] const Token* currToken() const;
        [[nodiscard]] bool check(const TokenType&);
        [[nodiscard]] bool checkNext(const TokenType&);
        [[nodiscard]] bool atEnd() const;
    };
}  // namespace Winter

template <>
struct std::formatter<Winter::Lexer> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const Winter::Lexer& lexer, std::format_context& ctx) const {
        std::string out = " --- Lexer --- \n";
        for (auto&& token : lexer.tokens) {
            out += token.toString() + "\n";
        }

        return std::format_to(ctx.out(), "{}", out);
    }
};

#endif  // WINTER_LEXER_H
