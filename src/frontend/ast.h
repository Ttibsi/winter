#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <cstdint>
#include <initializer_list>
#include <string>
#include <variant>
#include <vector>

namespace Winter {
    enum class NodeType : std::uint8_t {
        bodyNode,
        exprNode,
        funcNode,
        letNode,
        returnNode,
        paramNode,

        error
    };

    struct bodyNode;
    struct funcNode;
    struct letNode;
    struct paramNode;

    struct TOMBSTONE {};

    template <typename... Ts>
    struct _Node {
        NodeType type;
        std::variant<Ts...> data;
        std::vector<_Node> children;

        [[nodiscard]] explicit _Node(NodeType t, std::variant<Ts...> d)
            : type(t), data(d), children({}) {}

        [[nodiscard]] explicit _Node(
            NodeType t,
            std::variant<Ts...> d,
            std::initializer_list<_Node> c)
            : type(t), data(d), children(c) {}

        [[nodiscard]] static _Node tombstone() { return _Node(NodeType::error, TOMBSTONE()); }
    };

    using Node = _Node<bodyNode, letNode, funcNode, paramNode, TOMBSTONE>;

    struct bodyNode {
        int childCount;
    };

    struct exprNode {};

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

    struct returnNode {};

}  // namespace Winter

#endif  // WINTER_AST_H
