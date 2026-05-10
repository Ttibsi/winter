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
        boolNode,
        callNode,
        exprNode,
        forNode,
        funcNode,
        ifNode,
        letNode,
        numlitNode,
        paramNode,
        returnNode,
        strLitNode,
        varNode,

        error
    };

    struct argNode;
    struct boolNode;
    struct bodyNode;
    struct exprNode;
    struct forNode;
    struct funcNode;
    struct funcCallNode;
    struct ifNode;
    struct letNode;
    struct paramNode;
    struct numlitNode;
    struct returnNode;
    struct strLitNode;
    struct varNode;

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

        [[nodiscard]] bool operator==(this const _Node& self, const _Node& other) {
            return self.type == other.type && self.children.size() == other.children.size();
        };
    };

    using Node = _Node<
        argNode,
        boolNode,
        bodyNode,
        exprNode,
        forNode,
        letNode,
        funcNode,
        funcCallNode,
        ifNode,
        paramNode,
        numlitNode,
        returnNode,
        strLitNode,
        varNode,
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

    struct boolNode {
        bool val;

        [[nodiscard]] std::string display() const {
            return std::format("BoolNode[ value:{} ]", val);
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

    struct forNode {
        [[nodiscard]] std::string display() const { return "forNode[]"; }
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

    struct ifNode {
        std::size_t childCount;
        [[nodiscard]] std::string display() const {
            return std::format("ifNode[ children:{} ]", childCount);
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

    struct strLitNode {
        std::string value;

        [[nodiscard]] std::string display() const {
            return std::format("strLitNode[ value:{} ]", value);
        }
    };

    struct varNode {
        int childCount;
        std::string name;
        std::string type;

        [[nodiscard]] std::string display() const {
            return std::format("VarNode[ children:{}, name:{}, type:{} ]", childCount, name, type);
        }
    };

}  // namespace Winter

template <>
struct std::formatter<Winter::NodeType> {
    constexpr auto parse(std::format_parse_context& ctx) {
        auto it = ctx.begin();
        if (it != ctx.end() && *it != '}') {
            throw std::format_error("Invalid format specifier for NodeType");
        }
        return it;
    }

    auto format(Winter::NodeType node, std::format_context& ctx) const {
        switch (node) {
            case Winter::NodeType::argNode:    return std::format_to(ctx.out(), "argNode");
            case Winter::NodeType::bodyNode:   return std::format_to(ctx.out(), "bodyNode");
            case Winter::NodeType::boolNode:   return std::format_to(ctx.out(), "boolNode");
            case Winter::NodeType::callNode:   return std::format_to(ctx.out(), "callNode");
            case Winter::NodeType::exprNode:   return std::format_to(ctx.out(), "exprNode");
            case Winter::NodeType::funcNode:   return std::format_to(ctx.out(), "funcNode");
            case Winter::NodeType::forNode:    return std::format_to(ctx.out(), "forNode");
            case Winter::NodeType::ifNode:     return std::format_to(ctx.out(), "ifNode");
            case Winter::NodeType::letNode:    return std::format_to(ctx.out(), "letNode");
            case Winter::NodeType::numlitNode: return std::format_to(ctx.out(), "numlitNode");
            case Winter::NodeType::paramNode:  return std::format_to(ctx.out(), "paramNode");
            case Winter::NodeType::returnNode: return std::format_to(ctx.out(), "returnNode");
            case Winter::NodeType::strLitNode: return std::format_to(ctx.out(), "strLitNode");
            case Winter::NodeType::varNode:    return std::format_to(ctx.out(), "varNode");
            case Winter::NodeType::error:      return std::format_to(ctx.out(), "error");
        }
        return std::format_to(ctx.out(), "");
    }
};

#endif  // WINTER_AST_H
