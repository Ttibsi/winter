#ifndef WINTER_AST_NODES_H
#define WINTER_AST_NODES_H

#include <expected>
#include <memory>
#include <string>
#include <variant>
#include <vector>

#include "lexer.h"

// helper for std::visit
template <typename... Ts>
struct overloads : Ts... {
    using Ts::operator()...;
};

namespace Winter {
    using binding_t = std::size_t;

    enum class NodeType { BlockNode, ExprNode, FuncNode, RootNode, ReturnNode, ValueNode };

    class ASTNode {
       public:
        virtual std::string display(const std::size_t offset) const = 0;
        virtual std::string getNodeName() const = 0;
        virtual NodeType getNodeType() const = 0;
        virtual ~ASTNode() = default;
    };

    class BlockNode : public ASTNode {
       public:
        std::vector<std::unique_ptr<ASTNode>> stmts = {};

        [[nodiscard]] inline std::string display(const std::size_t offset) const override {
            std::string out = std::string(offset, ' ') + "BlockNode: {\n";
            for (auto&& stmt : stmts) { out += stmt->display(offset + 2); }

            out += std::string(offset, ' ') + "}\n";
            return out;
        }

        [[nodiscard]] inline std::string getNodeName() const override { return "BlockNode"; }
        [[nodiscard]] inline NodeType getNodeType() const override { return NodeType::BlockNode; }
    };

    class ExprNode : public ASTNode {
       public:
        const Token* op;
        std::unique_ptr<ASTNode> lhs = nullptr;
        std::unique_ptr<ASTNode> rhs = nullptr;
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {
            std::string out = std::string(offset, ' ') + "ExprNode { \n";
            if (lhs != nullptr) { out += lhs->display(offset + 2) + "\n"; }
            if (op != nullptr) { out += op->toString() + "\n"; }
            if (rhs != nullptr) { out += rhs->display(offset + 2) + "\n"; }

            out += std::string(offset, ' ') + "}";
            return out;
        }

        [[nodiscard]] inline std::string getNodeName() const override { return "ExprNode"; }
        [[nodiscard]] inline NodeType getNodeType() const override { return NodeType::ExprNode; }
    };

    class FuncNode : public ASTNode {
       public:
        std::string name;
        std::vector<Token> params;
        std::unique_ptr<BlockNode> body;

        explicit FuncNode() {}
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {
            std::string out = std::string(offset, ' ') + "FuncNode {\n";
            out += std::string(offset + 2, ' ') + "Name: " + name + "\n";
            for (auto&& tok : params) {
                out += std::string(offset + 2, ' ') + tok.toString() + "\n";
            }

            out += body->display(offset + 2);
            out += std::string(offset, ' ') + "}";
            return out;
        }

        [[nodiscard]] inline std::string getNodeName() const override { return "FuncNode"; }
        [[nodiscard]] inline NodeType getNodeType() const override { return NodeType::FuncNode; }
    };

    class RootNode : public ASTNode {
       public:
        std::vector<std::unique_ptr<ASTNode>> children = {};

        explicit RootNode() {}
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {
            std::string out = std::string(offset, ' ') + "RootNode {\n";
            for (auto&& child : children) { out += child->display(offset + 2) + "\n"; }
            out += "}";
            return out;
        }

        [[nodiscard]] inline std::string getNodeName() const override { return "RootNode"; }
        [[nodiscard]] inline NodeType getNodeType() const override { return NodeType::RootNode; }
    };

    class ReturnNode : public ASTNode {
       public:
        std::unique_ptr<ExprNode> expr;
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {
            std::string out = std::string(offset, ' ') + "ReturnNode {\n";
            out += expr->display(offset + 2) + "\n";
            out += std::string(offset, ' ') + "}\n";
            return out;
        }

        [[nodiscard]] inline std::string getNodeName() const override { return "ReturnNode"; }
        [[nodiscard]] inline NodeType getNodeType() const override { return NodeType::ReturnNode; }
    };

    class ValueNode : public ASTNode {
       public:
        std::variant<double> value;

        ValueNode(double v) : value(v) {}
        [[nodiscard]] inline std::string display(const std::size_t offset) const override {
            std::string out = std::string(offset, ' ') + "ValueNode {";
            std::visit(
                overloads {
                    [&out](double arg) { out += std::to_string(arg); },
                },
                value);
            out += "}";
            return out;
        }

        [[nodiscard]] inline std::string getNodeName() const override { return "ValueNode"; }
        [[nodiscard]] inline NodeType getNodeType() const override { return NodeType::ValueNode; }
    };

    template <typename T>
    using expected_node_t = std::expected<std::unique_ptr<T>, Err>;

};  // namespace Winter

#endif  // WINTER_AST_NODES_H
