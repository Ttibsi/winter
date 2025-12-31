#ifndef WINTER_AST_NODES_H
#define WINTER_AST_NODES_H

#include <expected>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "lexer.h"

namespace Winter {
    using binding_t = std::size_t;

    class ASTNode {
       public:
        virtual std::string display(const std::size_t offset) const;
        virtual ~ASTNode() = default;
    };

    class BlockNode : public ASTNode {
       public:
        std::vector<std::unique_ptr<ASTNode>> stmts = {};

        [[nodiscard]] inline std::string display(const std::size_t offset) const override {}
    };

    class ExprNode : public ASTNode {
       public:
        std::optional<TokenType> op = std::nullopt;
        std::unique_ptr<ASTNode> lhs = nullptr;
        std::unique_ptr<ASTNode> rhs = nullptr;
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {}
    };

    class FuncNode : public ASTNode {
       public:
        std::string name;
        std::vector<Token> params;
        std::unique_ptr<BlockNode> body;

        explicit FuncNode() {}
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {}
    };

    class RootNode : public ASTNode {
       public:
        std::vector<std::unique_ptr<ASTNode>> children = {};

        explicit RootNode() {}
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {}
    };

    class ReturnNode : public ASTNode {
       public:
        std::unique_ptr<ExprNode> expr;
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {}
    };

    class ValueNode : public ASTNode {
       public:
        std::variant<double> value;

        ValueNode(double v) : value(v) {}
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {}
    };

    template <typename T>
    using expected_node_t = std::expected<std::unique_ptr<T>, Err>;

};  // namespace Winter

#endif  // WINTER_AST_NODES_H
