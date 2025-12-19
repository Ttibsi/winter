#ifndef WINTER_PARSE_NODES_H
#define WINTER_PARSE_NODES_H

#include <optional>
#include <string>
#include <vector>

#include "lexer.h"

namespace Winter {
    enum class NodeType {
        BlockNode,
        ExprNode,
        FunctionNode,
        ReturnNode,
        ValueNode,
    };

    class ASTNode {
       public:
        [[nodiscard]] virtual NodeType getNodeType() const = 0;
        [[nodiscard]] virtual std::string getNodeName() const = 0;

        virtual constexpr ~ASTNode() = default;
    };

    class BlockNode : public ASTNode {
       public:
        std::vector<std::unique_ptr<ASTNode>> stmts = {};

        explicit BlockNode() {}
        [[nodiscard]] NodeType getNodeType() const override { return NodeType::BlockNode; }
        [[nodiscard]] std::string getNodeName() const override { return "BlockNode"; }
    };

    class ExprNode : public ASTNode {
       public:
        std::optional<TokenType> op;
        std::unique_ptr<ASTNode> lhs = nullptr;
        std::unique_ptr<ASTNode> rhs = nullptr;

        explicit ExprNode() {}
        [[nodiscard]] NodeType getNodeType() const override { return NodeType::ExprNode; }
        [[nodiscard]] std::string getNodeName() const override { return "ExprNode"; }
    };

    class FunctionNode : public ASTNode {
       public:
        std::string name;
        std::vector<Token> params;
        std::unique_ptr<BlockNode> body;

        explicit FunctionNode() : params({}) {}
        [[nodiscard]] NodeType getNodeType() const override { return NodeType::FunctionNode; }
        [[nodiscard]] std::string getNodeName() const override { return "FunctionNode"; }
    };

    class ReturnNode : public ASTNode {
       public:
        std::unique_ptr<ExprNode> expr;

        explicit ReturnNode() {}
        [[nodiscard]] NodeType getNodeType() const override { return NodeType::ReturnNode; }
        [[nodiscard]] std::string getNodeName() const override { return "ReturnNode"; }
    };

    class ValueNode : public ASTNode {
       public:
        double expr;

        explicit ValueNode(double e) : expr(e) {}
        [[nodiscard]] NodeType getNodeType() const override { return NodeType::ValueNode; }
        [[nodiscard]] std::string getNodeName() const override { return "ValueNode"; }
    };
}  // namespace Winter

#endif  // WINTER_PARSE_NODES_H
