#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <cstdint>
#include <format>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "lexer.h"

namespace Winter {
    enum class NodeType : std::uint8_t {
        argNode,
        bodyNode,
        callNode,
        exprNode,
        funcNode,
        letNode,
        numlitNode,
        returnNode,
        paramNode,

        error
    };

    struct argNode;
    struct bodyNode;
    struct exprNode;
    struct funcNode;
    struct funcCallNode;
    struct letNode;
    struct paramNode;
    struct numlitNode;
    struct returnNode;

    struct TOMBSTONE {
        [[nodiscard]] std::string display() const { return ""; }
    };

    template <typename... Ts>
    struct _Node {
        NodeType type;
        std::variant<Ts...> data;
        std::vector<_Node> children;

        [[nodiscard]] explicit _Node(NodeType t, std::variant<Ts...> d)
            : type(t), data(d), children({}) {}

        [[nodiscard]] explicit _Node(NodeType t, std::variant<Ts...> d, std::vector<_Node> c)
            : type(t), data(d), children(c) {}

        [[nodiscard]] static _Node tombstone() { return _Node(NodeType::error, TOMBSTONE()); }
    };

    using Node = _Node<
        argNode,
        bodyNode,
        exprNode,
        letNode,
        funcNode,
        funcCallNode,
        paramNode,
        numlitNode,
        returnNode,
        TOMBSTONE>;

    struct argNode {
        std::optional<std::string> str;
        std::optional<int> num;
        std::optional<char> ch;

        [[nodiscard]] std::string display() const {
            return std::format(
                "ArgNode[ str:{} num:{} char:{} ]", str.has_value() ? str.value() : "_",
                num.has_value() ? std::format("{}", num.value()) : "_",
                ch.has_value() ? std::format("{}", ch.value()) : "_");
        }
    };

    struct bodyNode {
        int childCount;

        [[nodiscard]] std::string display() const {
            return std::format("BodyNode[ count:{} ]", childCount);
        }
    };

    struct exprNode {
        int childCount;
        std::optional<TokenType> op;

        explicit exprNode(int c, TokenType tok) : childCount(c), op(tok) {}
        explicit exprNode(int c) : childCount(c), op(std::nullopt) {}

        [[nodiscard]] std::string display() const {
            return std::format(
                "ExprNode[ count:{}, op:{} ]", childCount,
                op.has_value() ? std::format("{}", op.value()) : "null");
        }
    };

    struct funcNode {
        static const int childCount = 1;
        std::vector<Node> parameters;
        std::string retType;

        [[nodiscard]] std::string display() const {
            return std::format("FuncNode[ params:{}, returnType:{} ]", parameters.size(), retType);
        }
    };

    struct funcCallNode {
        std::string name;

        [[nodiscard]] std::string display() const {
            return std::format("FuncCallNode[ name:{} ]", name);
        }
    };

    struct letNode {
        static const int childCount = 1;
        std::string name;
        bool isFunc;

        [[nodiscard]] std::string display() const {
            return std::format("LetNode[ name:{}, isFunc:{} ]", name, isFunc);
        }
    };

    struct paramNode {
        std::string name;
        std::string type;

        [[nodiscard]] std::string display() const {
            return std::format("ParamNode[ name:{}, type:{} ]", name, type);
        }
    };

    struct numlitNode {
        int value;

        [[nodiscard]] std::string display() const {
            return std::format("NumLitNode[ val:{} ]", value);
        }
    };

    struct returnNode {
        [[nodiscard]] std::string display() const { return std::format("ReturnNode[]"); }
    };

}  // namespace Winter

#endif  // WINTER_AST_H
