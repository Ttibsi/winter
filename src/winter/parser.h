#ifndef WINTER_PARSER_H
#define WINTER_PARSER_H

#include <memory>
#include <vector>

#include "ast_nodes.h"
#include "error.h"
#include "lexer.h"

namespace Winter {

    struct Parser {
        Lexer* L;
        std::vector<std::unique_ptr<ASTNode>> tree = {};

        [[nodiscard]] explicit Parser(Lexer* lexer_ptr) : L(lexer_ptr) {}
        [[nodiscard]] binding_t prefixBindingPower(const TokenType&) const;
        [[nodiscard]] binding_t infixBindingPower(const TokenType&) const;
        [[nodiscard]] expected_node_t<BlockNode> parseBlock();
        [[nodiscard]] expected_node_t<ExprNode> parseExpression(const binding_t);
        [[nodiscard]] expected_node_t<FuncNode> parseFunc();
        [[nodiscard]] expected_node_t<ReturnNode> parseReturn();
        [[nodiscard]] expected_node_t<ASTNode> parseStatement();
        [[nodiscard]] expected_node_t<RootNode> parse_tree();
    };

};  // namespace Winter

#endif  // WINTER_PARSER_H
