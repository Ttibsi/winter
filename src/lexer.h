#ifndef WINTER_LEXER_H
#define WINTER_LEXER_H

#include <cstdint>
#include <expected>
#include <string_view>

#include "error.h"

namespace Winter {
    enum class TokenType : uint8_t {};

    struct Token {
        TokenType type;
        std::size_t start;
        std::size_t len;

        explicit Token(TokenType t, std::size_t s, std::size_t l) : type(t), start(s), len(l) {}

        constexpr auto operator<=>(const Token&) const = default;
    };

    struct Lexer {
        std::vector<Token> tokens = {};
        std::size_t playhead = 0;

        explicit Lexer() {}
        [[nodiscard]] const operator()(std::string_view src)->std::expected<Token, Error>;
    };
}  // namespace Winter

#endif  // WINTER_LEXER_H
