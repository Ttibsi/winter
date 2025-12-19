#ifndef WINTER_PARSER_H
#define WINTER_PARSER_H

#include <expected>
#include <memory>
#include <vector>

#include "ast_nodes.h"
#include "error.h"
#include "lexer.h"

namespace Winter {
    using binding_t = unsigned int;

    struct Parser {
        std::unique_ptr<Lexer> L;
        std::vector<std::unique_ptr<ASTNode>> tree = {};

        explicit Parser(std::unique_ptr<Lexer> lexer_ptr) : L(std::move(lexer_ptr)) {}

        [[nodiscard]] binding_t prefixBindingPower(const TokenType&);
        [[nodiscard]] binding_t infixBindingPower(const TokenType&);
        [[nodiscard]] std::expected<std::unique_ptr<FunctionNode>, Err> parseFunc();
        [[nodiscard]] std::expected<std::unique_ptr<BlockNode>, Err> parseBlock();
        [[nodiscard]] std::expected<std::unique_ptr<ASTNode>, Err> parseStatement();
        [[nodiscard]] std::expected<std::unique_ptr<ReturnNode>, Err> parseReturn();
        [[nodiscard]] std::expected<std::unique_ptr<ExprNode>, Err> parseExpression(binding_t);
        [[nodiscard]] result_t parseTree();
    };
}  // namespace Winter

#endif  // WINTER_PARSER_H
