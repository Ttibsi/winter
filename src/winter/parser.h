#ifndef WINTER_PARSER_H
#define WINTER_PARSER_H

#include <expected>
#include <format>
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
        [[nodiscard]] std::expected<FunctionNode_ptr, Err> parseFunc();
        [[nodiscard]] std::expected<BlockNode_ptr, Err> parseBlock();
        [[nodiscard]] std::expected<ASTNode_ptr, Err> parseStatement();
        [[nodiscard]] std::expected<ReturnNode_ptr, Err> parseReturn();
        [[nodiscard]] std::expected<ExprNode_ptr, Err> parseExpression(binding_t);
        [[nodiscard]] result_t parseTree();
    };
}  // namespace Winter

template <>
struct std::formatter<Winter::Parser> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const Winter::Parser& parser, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "TODO: Parser formatting");
    }
};

#endif  // WINTER_PARSER_H
