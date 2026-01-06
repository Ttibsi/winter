#ifndef WINTER_BYTECODE_H
#define WINTER_BYTECODE_H

#include <expected>
#include <format>
#include <memory>

#include "ast_nodes.h"
#include "error.h"

namespace Winter {
    enum class Opcode { OP_NULL };

    struct Chunk {
        Opcode op;
    };

    struct Module {
        std::string name;
        std::vector<Chunk> chunks = {};

        explicit Module() : name("") {}
        explicit Module(std::string inp) : name(inp) {}
    };

    struct Generator {
        std::unique_ptr<RootNode> ast_node;

        explicit Generator(std::unique_ptr<RootNode> root) : ast_node(std::move(root)) {}
        [[nodiscard]] std::expected<Module, Err> generate();
    };
};  // namespace Winter

template <>
struct std::formatter<Winter::Module> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const Winter::Module& mod, std::format_context& ctx) const {
        std::string out = " --- Module --- \n";
        return std::format_to(ctx.out(), "{}", out);
    }
};

#endif  // WINTER_BYTECODE_H
