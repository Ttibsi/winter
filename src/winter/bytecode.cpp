#include "bytecode.h"

namespace Winter {
    [[nodiscard]] expected_chunk_t Generator::compileFunc() {
        return Err::TODO();
    }

    [[nodiscard]] std::expected<Module, Err> Generator::generate() {
        Module mod = Module("main");

        if (ast_node->getNodeType() != NodeType::RootNode) {
            return std::unexpected(
                Err(ErrType::BytecodeGenerationError, "Node provided not Root node"));
        }

        for (auto&& child : ast_node->children) {
            switch (child->getNodeType()) {
                case (NodeType::FuncNode):
                    expected_chunk_t func = compileFunc();
                    if (!func.has_value()) { return std::unexpected(func.error()); }
                    mod.chunks.push_back(func.value());
                    break;
            }
        };

        return mod;
    }

}  // namespace Winter
