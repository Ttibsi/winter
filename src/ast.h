#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace Winter {
    enum class NodeType : uint8_t {
        literal,
        moduleDef,
        alias,
        Class
    };

    struct Literal {
        float value;
    };

    struct ModuleDefinition {
        std::string name;
    };

    struct Alias {
        std::string name;
        std::string type;
    };

    struct AttrDef {};
    struct MethodDef {};
    using AttrMethod = std::variant<AttrDef, MethodDef>;

    struct ClassDef {
        std::vector<> generics = {};
        std::vector<AttrDef> attributes = {};
        std::vector<MethodDef> methods = {};
    };

    struct EnumDef {};
    struct FuncDef {};
    struct InterfaceDef {};

    using TypeInner = std::variant<ClassDef, EnumDef, InterfaceDef>;

    struct typeDef {
        std::string name;
        TypeInner def;
    };
    using Payload = std::variant<Literal, ModuleDefinition, Alias, ClassDef>;

    struct Node {
        NodeType type;
        Payload payload;
    };

    struct AST {
        std::vector<Node> nodes = {};

        // [[nodiscard]] constexpr auto makeLit(float v) -> std::size_t {
        //     nodes.emplace_back(Type::literal, Literal(v));
        //     return nodes.size() - 1;
        // }

        [[nodiscard]] constexpr auto makeMod(std::string v) -> std::size_t {
            nodes.emplace_back(NodeType::moduleDef, ModuleDefinition(v));
            return nodes.size() - 1;
        }

        [[nodiscard]] constexpr auto makeAlias(std::string name, std::string type) -> std::size_t {
            nodes.emplace_back(NodeType::alias, Alias(name, type));
            return nodes.size() - 1;
        }

        [[nodiscard]] constexpr auto makeClass(ClassDef cls) -> std::size_t {
            nodes.emplace_back(NodeType::Class, cls);
            return nodes.size() - 1;
        }
    };

}  // namespace Winter

#endif  // WINTER_AST_H
