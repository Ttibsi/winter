#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "lexer.h"

namespace Winter {
    enum class NodeType : std::uint8_t {
        bodyNode,
        exprNode,
        funcNode,
        letNode,
        numlitNode,
        returnNode,
        paramNode,

        error
    };

    struct bodyNode;
    struct exprNode;
    struct funcNode;
    struct letNode;
    struct paramNode;
    struct numlitNode;
    struct returnNode;

    struct TOMBSTONE {};

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

    using Node =
        _Node<bodyNode, exprNode, letNode, funcNode, paramNode, numlitNode, returnNode, TOMBSTONE>;

    struct bodyNode {
        int childCount;
    };

    struct exprNode {
        int childCount;
        std::optional<TokenType> op;

        explicit exprNode(int c, TokenType tok) : childCount(c), op(tok) {}
        explicit exprNode(int c) : childCount(c), op(std::nullopt) {}
    };

    struct funcNode {
        static const int childCount = 1;
        std::vector<Node> parameters;
        std::string retType;
    };

    struct letNode {
        static const int childCount = 1;
        std::string name;
        bool isFunc;
    };

    struct paramNode {
        std::string name;
        std::string type;
    };

    struct numlitNode {
        int value;
    };

    struct returnNode {};

}  // namespace Winter

#endif  // WINTER_AST_H
