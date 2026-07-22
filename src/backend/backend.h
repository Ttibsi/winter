#ifndef WINTER_BACKEND_H
#define WINTER_BACKEND_H

#include <expected>
#include <memory>
#include <optional>
#include <span>
#include <string_view>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "../error.h"
#include "../frontend/parser.h"
#include "llvm/Target/TargetMachine.h"

namespace Winter {
    using namespace llvm;
    using module_result_t = std::expected<std::unique_ptr<Module>, Error>;
    using module_ptr_t = std::unique_ptr<Module>;

    struct Backend {
        LLVMContext ctx;
        Node currentNode;

        Backend() : currentNode(Node::tombstone()) {}
        [[nodiscard]] std::expected<Type*, Error> getType(std::string_view);
        [[nodiscard]] std::expected<const Target*, Error> getTarget();
        [[nodiscard]] std::optional<Error> createFunction(module_ptr_t&, const letNode*);
        [[nodiscard]] BasicBlock* createBlock(module_ptr_t&, const letNode*);
        [[nodiscard]] Value* compileExpression(IRBuilder<>*);
        void populateBlock(BasicBlock*);
        [[nodiscard]] module_result_t compileModule(std::span<Node>);
        void display_module(module_ptr_t& mod) const;
        [[nodiscard]] std::optional<Error> outputObjectFile();
    };
}  // namespace Winter

#endif  // WINTER_BACKEND_H
