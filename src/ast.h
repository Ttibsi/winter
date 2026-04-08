#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <cstdint>
#include <string>
#include <variant>
#include <vector>

namespace Winter {
    enum class Type : uint8_t {
        literal,
        moduleDef,
        alias
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

    using Payload = std::variant<Literal, ModuleDefinition, Alias>;

    struct Node {
        Type type;
        Payload payload;
    };

    struct AST {
        std::vector<Node> nodes = {};

        // [[nodiscard]] constexpr auto makeLit(float v) -> std::size_t {
        //     nodes.emplace_back(Type::literal, Literal(v));
        //     return nodes.size() - 1;
        // }

        [[nodiscard]] constexpr auto makeMod(std::string v) -> std::size_t {
            nodes.emplace_back(Type::moduleDef, ModuleDefinition(v));
            return nodes.size() - 1;
        }

        [[nodiscard]] constexpr auto makeAlias(std::string name, std::string type) -> std::size_t {
            nodes.emplace_back(Type::alias, Alias(name, type));
            return nodes.size() - 1;
        }
    };

}  // namespace Winter

#endif  // WINTER_AST_H
