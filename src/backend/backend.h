#ifndef WINTER_BACKEND_H
#define WINTER_BACKEND_H

#include <expected>
#include <memory>
#include <optional>
#include <span>

#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Module.h>

#include "../error.h"
#include "../frontend/parser.h"

namespace Winter {
    using namespace llvm;
    using module_result_t = std::expected<std::unique_ptr<Module>, Error>;
    using module_ptr_t = std::unique_ptr<Module>;

    [[nodiscard]] std::expected<Type*, Error> getType(LLVMContext&, std::string_view);
    [[nodiscard]] std::optional<Error>
    createFunction(LLVMContext&, module_ptr_t&, Node, const letNode*);
    [[nodiscard]] BasicBlock* createBlock(LLVMContext&, module_ptr_t&, const letNode*);
    [[nodiscard]] Value* compileExpression(LLVMContext&, IRBuilder<>*, Node);
    void populateBlock(LLVMContext&, BasicBlock*, const Node);
    [[nodiscard]] module_result_t compileModule(LLVMContext&, std::span<Node>);
}  // namespace Winter

#endif  // WINTER_BACKEND_H
