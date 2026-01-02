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
        std::unique_ptr<RootNode> root = nullptr;

        [[nodiscard]] explicit Parser(Lexer* lexer_ptr) : L(lexer_ptr) {}
        [[nodiscard]] binding_t prefixBindingPower(const TokenType&) const;
        [[nodiscard]] binding_t infixBindingPower(const TokenType&) const;
        [[nodiscard]] expected_node_t<BlockNode> parseBlock();
        [[nodiscard]] expected_node_t<ExprNode> parseExpression(const binding_t);
        [[nodiscard]] expected_node_t<FuncNode> parseFunc();
        [[nodiscard]] expected_node_t<ReturnNode> parseReturn();
        [[nodiscard]] expected_node_t<ASTNode> parseStatement();
        [[nodiscard]] result_t parse_tree();
    };

};  // namespace Winter

template <>
struct std::formatter<Winter::Parser> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const Winter::Parser& parser, std::format_context& ctx) const {
        std::string out = " --- Parser --- \n";
        if (parser.root == nullptr) {
            return std::format_to(ctx.out(), "{}", out);
        }

        for (auto&& child : parser.root->children) {
            out += child->display(0);
            out += "\n";
        }

        return std::format_to(ctx.out(), "{}", out);
    }
};

#endif  // WINTER_PARSER_H
