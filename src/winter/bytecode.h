#ifndef WINTER_BYTECODE_H
#define WINTER_BYTECODE_H

#include <expected>
#include <format>
#include <optional>
#include <string>
#include <vector>

#include "ast_nodes.h"
#include "error.h"
#include "lexer.h"

namespace Winter {
    enum class Opcode {
        ADD,
        DIV,
        MUL,
        NIL,
        RET,
        STORE_CONST,  // Used to store doubles inline
        STORE_STACK,  // Used to store any other value in a local stack
        SUB,
    };

    struct Bytecode {
        Opcode op;
        double operand;

        explicit Bytecode(Opcode code) : op(code), operand(0.0) {}
        explicit Bytecode(Opcode code, double operand) : op(code), operand(operand) {}
    };

    struct Chunk {
        std::string name;
        std::vector<Bytecode> instructions = {};

        explicit Chunk() : name("") {}
        explicit Chunk(const std::string& inp) : name(inp) {}

        inline void extend(const std::vector<Bytecode>& input) {
            instructions.insert(instructions.end(), input.begin(), input.end());
        }
    };

    struct Module {
        std::string name;
        std::vector<Chunk> chunks = {};

        explicit Module() : name("") {}
        explicit Module(std::string inp) : name(inp) {}
    };

    using expected_bytecode_t = std::expected<std::vector<Bytecode>, Err>;
    using expected_chunk_t = std::expected<Chunk, Err>;

    struct Generator {
        std::unique_ptr<RootNode> ast_node;

        explicit Generator(std::unique_ptr<RootNode> root) : ast_node(std::move(root)) {}
        [[nodiscard]] std::expected<Bytecode, Err> compileValue(ValueNode*);
        [[nodiscard]] std::optional<Bytecode> compileTok(const Token*);
        [[nodiscard]] expected_bytecode_t compileExpression(ExprNode*);
        [[nodiscard]] expected_bytecode_t compileReturn(const ReturnNode*);
        [[nodiscard]] expected_chunk_t compileFunc(const FuncNode*);
        [[nodiscard]] std::expected<Module, Err> generate();
    };

};  // namespace Winter

template <>
struct std::formatter<Winter::Chunk> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const Winter::Chunk& chunk, std::format_context& ctx) const {
        std::string out = "(chunk) TODO";
        return std::format_to(ctx.out(), "{}", out);
    }
};

template <>
struct std::formatter<Winter::Module> {
    constexpr auto parse(std::format_parse_context& ctx) { return ctx.begin(); }
    auto format(const Winter::Module& mod, std::format_context& ctx) const {
        std::string out = " --- Module --- \n";
        for (auto&& chunk : mod.chunks) { out += std::format("{}\n", chunk); }

        out += "\n";
        return std::format_to(ctx.out(), "{}", out);
    }
};

#endif  // WINTER_BYTECODE_H
