#ifndef WINTER_PARSER_H
#define WINTER_PARSER_H

#include <expected>
#include <initializer_list>
#include <memory>
#include <string_view>
#include <variant>
#include <vector>

#include "../error.h"
#include "ast.h"
#include "lexer.h"

namespace Winter {
    using Node_Result = std::expected<Node, Error>;

    struct Parser {
        Lexer L;
        Token current;
        Token prev;

        explicit Parser(std::string_view src)
            : L(Lexer(src)), current(Token::tombstone()), prev(Token::tombstone()) {}
        [[nodiscard]] bool check(const TokenType&) const noexcept;
        void consume() noexcept;
        [[nodiscard]] bool consume(std::initializer_list<TokenType> tokens) noexcept;

        [[nodiscard]] Node_Result parseParam() noexcept;
        [[nodiscard]] Node_Result parseBody() noexcept;
        [[nodiscard]] Node_Result parseFunc() noexcept;
        [[nodiscard]] Node_Result parseLet() noexcept;

        [[nodiscard]] std::expected<std::vector<Node>, Error> operator()();
        void display_syntax_tree(const std::vector<Node>&) const noexcept;
    };

}  // namespace Winter

#endif  // WINTER_PARSER_H
