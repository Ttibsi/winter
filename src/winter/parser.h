#ifndef WINTER_PARSER_H
#define WINTER_PARSER_H

#include <format>
#include <vector>

#include "error.h"
#include "lexer.h"

namespace Winter {
    enum class NodeType { Func };

    struct ASTNode;

    struct ASTNode {
        NodeType type;

        union {
            uint64_t numeric;
        };

        std::vector<ASTNode*> children = {};
    };

    struct Parser {
        Lexer* L;
        std::vector<ASTNode*> tree;

        explicit Parser(Lexer* lexer) : L(lexer), tree({}) {}
        [[nodiscard]] const result_t parseTree();
    };
};  // namespace Winter

template <>
struct std::formatter<Winter::Parser> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const Winter::Parser& parser, std::format_context& ctx) const {
        return std::format_to(ctx.out(), "TODO: Parser formatting");
    }
};

#endif  // WINTER_PARSER_H
