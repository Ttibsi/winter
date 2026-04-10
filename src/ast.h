#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <cstdint>
#include <optional>
#include <string>
#include <variant>
#include <vector>

namespace Winter {
    enum class NodeType : uint8_t {
        literal,
        moduleDef,
        alias,
        Class,
        Enum,
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
        std::optional<std::string> interface = std::nullopt;
        std::vector<> generics = {};
        std::vector<AttrDef> attributes = {};
        std::vector<MethodDef> methods = {};
    };

    struct EnumDef {
        std::vector<std::string> enumerations;
    };

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

        [[nodiscard]] constexpr auto makeEnum(std::vector<std::string> enums) -> std::size_t {
            nodes.emplace_back(NodeType::Enum, EnumDef(enums));
            return nodes.size() - 1;
        }
    };

}  // namespace Winter

#endif  // WINTER_AST_H
