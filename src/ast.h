#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <cstdint>
#include <variant>
#include <vector>

namespace Winter {
    enum class Type : uint8_t {
        literal
    };

    struct Literal {
        float value;
    };

    using Payload = std::variant<Literal>;

    struct Node {
        Type type;
        Payload payload;
    };

    struct AST {
        std::vector<Node> nodes = {};

        [[nodiscard]] constexpr auto make_lit(float v) -> std::size_t {
            nodes.emplace_back(Type::literal, Literal(v));
            return nodes.size() - 1;
        }
    };
}  // namespace Winter

#endif  // WINTER_AST_H
