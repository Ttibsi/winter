#ifndef WINTER_PARSER_H
#define WINTER_PARSER_H

#include <expected>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include "../error.h"
#include "ast.h"
#include "lexer.h"

namespace Winter {
    struct Parser {
        Lexer L;
        Token current;
        Token prev;

        explicit Parser(std::string_view src)
            : L(Lexer(src)), current(Token::tombstone()), prev(Token::tombstone()) {}
        [[nodiscard]] bool check(const TokenType&) const noexcept;
        void consume() noexcept;
        [[nodiscard]] bool consume(std::same_as<TokenType> auto... tokens) noexcept;

        [[nodiscard]] std::expected<Stmt, Error> parseStmt() noexcept;

        [[nodiscard]] std::expected<std::vector<Stmt>, Error> operator()();
    };
}  // namespace Winter

#endif  // WINTER_PARSER_H
