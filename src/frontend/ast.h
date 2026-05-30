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
        aliasNode,
        argNode,
        bodyNode,
        boolNode,
        callNode,
        caseNode,
        classNode,
        charLitNode,
        enumNode,
        exprNode,
        forNode,
        funcAlias,
        funcNode,
        identNode,
        ifNode,
        interfaceNode,
        letNode,
        modNode,
        numlitNode,
        paramNode,
        returnNode,
        strLitNode,
        switchNode,
        typeAlias,
        typeNode,
        varNode,

        error
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
            case Winter::NodeType::aliasNode:     return std::format_to(ctx.out(), "aliasNode");
            case Winter::NodeType::argNode:       return std::format_to(ctx.out(), "argNode");
            case Winter::NodeType::bodyNode:      return std::format_to(ctx.out(), "bodyNode");
            case Winter::NodeType::boolNode:      return std::format_to(ctx.out(), "boolNode");
            case Winter::NodeType::callNode:      return std::format_to(ctx.out(), "callNode");
            case Winter::NodeType::caseNode:      return std::format_to(ctx.out(), "caseNode");
            case Winter::NodeType::charLitNode:   return std::format_to(ctx.out(), "charLitNode");
            case Winter::NodeType::classNode:     return std::format_to(ctx.out(), "classNode");
            case Winter::NodeType::enumNode:      return std::format_to(ctx.out(), "enumNode");
            case Winter::NodeType::exprNode:      return std::format_to(ctx.out(), "exprNode");
            case Winter::NodeType::forNode:       return std::format_to(ctx.out(), "forNode");
            case Winter::NodeType::funcAlias:     return std::format_to(ctx.out(), "funcAlias");
            case Winter::NodeType::funcNode:      return std::format_to(ctx.out(), "funcNode");
            case Winter::NodeType::identNode:     return std::format_to(ctx.out(), "identNode");
            case Winter::NodeType::ifNode:        return std::format_to(ctx.out(), "ifNode");
            case Winter::NodeType::interfaceNode: return std::format_to(ctx.out(), "interfaceNode");
            case Winter::NodeType::letNode:       return std::format_to(ctx.out(), "letNode");
            case Winter::NodeType::modNode:       return std::format_to(ctx.out(), "modNode");
            case Winter::NodeType::numlitNode:    return std::format_to(ctx.out(), "numlitNode");
            case Winter::NodeType::paramNode:     return std::format_to(ctx.out(), "paramNode");
            case Winter::NodeType::returnNode:    return std::format_to(ctx.out(), "returnNode");
            case Winter::NodeType::strLitNode:    return std::format_to(ctx.out(), "strLitNode");
            case Winter::NodeType::switchNode:    return std::format_to(ctx.out(), "switchNode");
            case Winter::NodeType::typeAlias:     return std::format_to(ctx.out(), "typeAlias");
            case Winter::NodeType::typeNode:      return std::format_to(ctx.out(), "typeNode");
            case Winter::NodeType::varNode:       return std::format_to(ctx.out(), "varNode");
            case Winter::NodeType::error:         return std::format_to(ctx.out(), "error");
        }
        return std::format_to(ctx.out(), "");
    }
};

namespace Winter {
    struct aliasNode;
    struct typeAlias;
    struct funcAlias;
    struct argNode;
    struct boolNode;
    struct bodyNode;
    struct caseNode;
    struct classNode;
    struct charLitNode;
    struct enumNode;
    struct exprNode;
    struct forNode;
    struct funcNode;
    struct funcCallNode;
    struct identNode;
    struct ifNode;
    struct interfaceNode;
    struct letNode;
    struct modNode;
    struct paramNode;
    struct numlitNode;
    struct returnNode;
    struct strLitNode;
    struct switchNode;
    struct typeNode;
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
        aliasNode,
        typeAlias,
        funcAlias,
        argNode,
        bodyNode,
        boolNode,
        caseNode,
        classNode,
        charLitNode,
        enumNode,
        exprNode,
        forNode,
        funcCallNode,
        funcNode,
        identNode,
        ifNode,
        interfaceNode,
        letNode,
        modNode,
        numlitNode,
        paramNode,
        returnNode,
        strLitNode,
        switchNode,
        typeNode,
        varNode,
        TOMBSTONE>;

    struct aliasNode {
        enum class childType {
            type,
            func
        };
        std::string ident;
        childType tag;

        [[nodiscard]] std::string display() const {
            return std::format(
                "AliasNode[ ident:{}, tag:{} ]", ident, tag == childType::type ? "type" : "func");
        }
    };

    struct typeAlias {
        std::string type;

        [[nodiscard]] std::string display() const {
            return std::format("typeAlias[ type:{} ]", type);
        }
    };

    struct funcAlias {
        int paramCount;

        [[nodiscard]] std::string display() const {
            return std::format("funcAlias[ paramCount:{} ]", paramCount);
        }
    };

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

    struct caseNode {
        bool fallthrough;  // falls through to another node as a child
        std::string ident;
        bool defaultCase;

        [[nodiscard]] std::string display() const {
            if (defaultCase) {
                return std::format("CaseNode[ fallthrough:{}, DEFAULT ]", fallthrough);
            }
            return std::format("CaseNode[ fallthrough:{}, ident:{} ]", fallthrough, ident);
        }
    };

    struct charLitNode {
        char value;

        [[nodiscard]] std::string display() const {
            return std::format("CharLitNode[ val:{} ]", value);
        }
    };

    struct classNode {
        int attrCount;
        int methodCount;

        [[nodiscard]] std::string display() const {
            return std::format("ClassNode[ attributes:{}, methods:{} ]", attrCount, methodCount);
        }
    };

    struct enumNode {
        int count;

        [[nodiscard]] std::string display() const {
            return std::format("EnumNode[ count:{} ]", count);
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
        int childCount;
        std::string name;
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

    struct identNode {
        std::string value;

        [[nodiscard]] std::string display() const {
            return std::format("identNode[ value:{} ]", value);
        }
    };

    struct ifNode {
        std::size_t childCount;

        [[nodiscard]] std::string display() const {
            return std::format("ifNode[ children:{} ]", childCount);
        }
    };

    struct interfaceNode {
        int attrCount;
        int methodCount;

        [[nodiscard]] std::string display() const {
            return std::format("interfaceNode[ attrs:{}, methods:{} ]", attrCount, methodCount);
        }
    };

    struct letNode {
        static const int childCount = 1;
        std::string name;
        bool isFunc;
        bool isConst;

        [[nodiscard]] std::string display() const {
            return std::format("LetNode[ name:{}, isFunc:{}, isConst:{} ]", name, isFunc, isConst);
        }
    };

    struct modNode {
        std::string name;

        [[nodiscard]] std::string display() const {
            return std::format("ModNode[ name:{} ]", name);
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
        [[nodiscard]] std::string display() const { return "ReturnNode[]"; }
    };

    struct strLitNode {
        std::string value;

        [[nodiscard]] std::string display() const {
            return std::format("strLitNode[ value:{} ]", value);
        }
    };

    struct switchNode {
        std::string ident;
        int caseCount;     // including default
        bool defaultCase;  // included in children

        [[nodiscard]] std::string display() const {
            return std::format(
                "SwitchNode[ ident:{}, cases:{}, defaultCase:{} ]", ident, caseCount, defaultCase);
        }
    };

    struct typeNode {
        NodeType child;

        [[nodiscard]] std::string display() const {
            return std::format("typeNode[ child:{} ]", child);
        }
    };

    struct varNode {
        int childCount;
        std::string name;
        std::string type;
        bool isConst;

        [[nodiscard]] std::string display() const {
            return std::format(
                "VarNode[ children:{}, name:{}, type:{}, const:{}]", childCount, name, type,
                isConst);
        }
    };

}  // namespace Winter

#endif  // WINTER_AST_H
