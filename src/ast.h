#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <cstdint>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace Winter {
    enum class NodeType : uint8_t {
        literal,
        moduleDef,
        alias,
        Class,
        Enum,
        Function,
        Typedef,
        Let,
        If,
        For,
        Switch,
        Interface
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

    struct ClassDef {
        std::optional<std::string> interface = std::nullopt;
        std::vector<std::string> generics = {};
        std::vector<std::size_t> members = {};
    };

    struct AttrDef {};
    struct MethodDef {};
    using AttrMethod = std::variant<AttrDef, MethodDef>;

    struct EnumDef {
        std::vector<std::string> enumerations;
    };

    struct FuncDef {
        std::vector<std::string> generics = {};
        std::vector<std::pair<std::string, std::string>> args = {};
        std::vector<std::size_t> body = {};
        std::string return_type;
    };

    struct InterfaceDef {
        std::vector<std::pair<std::string, std::string>> attributes;
        std::vector<std::pair<std::string, FuncDef>> methods;
    };

    using TypeInner = std::variant<ClassDef, EnumDef, InterfaceDef>;

    struct typeDef {
        std::string name;
        std::size_t inner_id;
    };

    struct LetDef {
        std::string name;
        std::size_t inner_id;
        std::optional<std::string> type_annotation = std::nullopt;
    };

    struct IfDef {
        std::size_t cond = 0;
        std::vector<std::size_t> then_body = {};
        std::optional<std::vector<std::size_t>> else_body = std::nullopt;
    };

    struct CStyleFor {
        std::optional<std::size_t> init = std::nullopt;
        std::optional<std::size_t> cond = std::nullopt;
        std::optional<std::size_t> iter = std::nullopt;
        std::vector<std::size_t> body = {};
    };

    struct ForeachFor {
        std::string var;
        std::size_t iterable = 0;
        std::vector<std::size_t> body = {};
    };

    using ForDef = std::variant<CStyleFor, ForeachFor>;

    struct CaseArm {
        std::size_t label = 0;
        std::vector<std::size_t> body = {};
    };

    struct SwitchDef {
        std::size_t scrutinee = 0;
        std::vector<CaseArm> cases = {};
        std::optional<std::vector<std::size_t>> default_body = std::nullopt;
    };

    using Payload = std::variant<
        Literal,
        ModuleDefinition,
        Alias,
        TypeInner,
        FuncDef,
        typeDef,
        LetDef,
        IfDef,
        ForDef,
        SwitchDef,
        InterfaceDef>;

    struct Node {
        NodeType type;
        Payload payload;
    };

    struct AST {
        std::vector<Node> nodes = {};

        [[maybe_unused]] constexpr auto makeMod(std::string v) -> std::size_t {
            nodes.emplace_back(NodeType::moduleDef, ModuleDefinition(v));
            return nodes.size() - 1;
        }

        [[maybe_unused]] constexpr auto makeAlias(std::string name, std::string type)
            -> std::size_t {
            nodes.emplace_back(NodeType::alias, Alias(name, type));
            return nodes.size() - 1;
        }

        [[maybe_unused]] constexpr auto makeClass(ClassDef cls) -> std::size_t {
            nodes.emplace_back(NodeType::Class, cls);
            return nodes.size() - 1;
        }

        [[maybe_unused]] constexpr auto makeEnum(std::vector<std::string> enums) -> std::size_t {
            nodes.emplace_back(NodeType::Enum, EnumDef(enums));
            return nodes.size() - 1;
        }

        [[maybe_unused]] constexpr auto makeFunc(FuncDef func) -> std::size_t {
            nodes.emplace_back(NodeType::Function, func);
            return nodes.size() - 1;
        }

        [[maybe_unused]] constexpr auto makeTypeDef(std::string name, std::size_t inner_idx)
            -> std::size_t {
            nodes.emplace_back(NodeType::Typedef, typeDef(name, inner_idx));
            return nodes.size() - 1;
        }

        [[maybe_unused]] constexpr auto makeLet(
            std::string name,
            std::size_t idx,
            std::optional<std::string> type = std::nullopt) -> std::size_t {
            nodes.emplace_back(NodeType::Let, LetDef {std::move(name), idx, std::move(type)});
            return nodes.size() - 1;
        }

        [[nodiscard]] constexpr auto makeIf(IfDef if_stmt) -> std::size_t {
            nodes.emplace_back(NodeType::If, std::move(if_stmt));
            return nodes.size() - 1;
        }

        [[nodiscard]] constexpr auto makeFor(ForDef for_stmt) -> std::size_t {
            nodes.emplace_back(NodeType::For, std::move(for_stmt));
            return nodes.size() - 1;
        }

        [[nodiscard]] constexpr auto makeSwitch(SwitchDef sw) -> std::size_t {
            nodes.emplace_back(NodeType::Switch, std::move(sw));
            return nodes.size() - 1;
        }

        [[nodiscard]] constexpr auto makeInterface(InterfaceDef iface) -> std::size_t {
            nodes.emplace_back(NodeType::Interface, iface);
            return nodes.size() - 1;
        }
    };

}  // namespace Winter

#endif  // WINTER_AST_H
