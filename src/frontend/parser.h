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

        const std::unordered_map<TokenType, std::size_t> infixBindingPower = {
            {TokenType::dot, 9},       {TokenType::lparen, 9},     {TokenType::rparen, 9},
            {TokenType::star, 7},      {TokenType::slash, 7},      {TokenType::plus, 6},
            {TokenType::minus, 6},     {TokenType::op_greater, 5}, {TokenType::op_greater_eq, 5},
            {TokenType::op_less, 5},   {TokenType::op_less_eq, 5}, {TokenType::op_equal_eq, 4},
            {TokenType::op_not_eq, 4}, {TokenType::op_and, 3},     {TokenType::op_or, 2},
            {TokenType::op_equal, 1}};

        explicit Parser(std::string_view src)
            : L(Lexer(src)), current(Token::tombstone()), prev(Token::tombstone()) {}
        [[nodiscard]] bool check(const TokenType&) const noexcept;
        void consume() noexcept;
        [[nodiscard]] bool consume(std::initializer_list<TokenType> tokens) noexcept;

        [[nodiscard]] Node_Result parseArg() noexcept;
        [[nodiscard]] Node_Result parseBody() noexcept;
        [[nodiscard]] Node_Result parseCallOrVariable() noexcept;
        [[nodiscard]] Node_Result parseExpr(std::size_t) noexcept;
        [[nodiscard]] Node_Result parseFunc() noexcept;
        [[nodiscard]] Node_Result parseFuncCall() noexcept;
        [[nodiscard]] Node_Result parseLet() noexcept;
        [[nodiscard]] Node_Result parseNumLit() noexcept;
        [[nodiscard]] Node_Result parseParam() noexcept;
        [[nodiscard]] Node_Result parseReturn() noexcept;
        [[nodiscard]] Node_Result parseVariable() noexcept;

        [[nodiscard]] std::expected<std::vector<Node>, Error> operator()();
        void display_syntax_tree(const std::vector<Node>&) const noexcept;
    };

}  // namespace Winter

#endif  // WINTER_PARSER_H
