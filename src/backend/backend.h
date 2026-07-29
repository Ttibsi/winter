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
        std::optional<Triple> targetTriple = std::nullopt;
        std::string_view file_name;

        Backend(std::string_view fName) : currentNode(Node::tombstone()), file_name(fName) {}
        [[nodiscard]] std::expected<Type*, Error> getType(std::string_view);
        [[nodiscard]] std::expected<const Target*, Error> getTarget();
        [[nodiscard]] std::optional<Error> createFunction(module_ptr_t&, const letNode*);
        [[nodiscard]] BasicBlock* createBlock(module_ptr_t&, const letNode*);
        [[nodiscard]] Value* compileExpression(IRBuilder<>*);
        [[nodiscard]] Value* compileNumLit();
        void populateBlock(BasicBlock*);
        void insertStart(module_ptr_t&);
        [[nodiscard]] module_result_t compileModule(std::span<Node>);
        void display_module(module_ptr_t&) const;
        void emitBitcodeFile(module_ptr_t&) const;
        [[nodiscard]] std::expected<std::string, Error> outputObjectFile(module_ptr_t&);
        [[nodiscard]] std::optional<Error> linkModules(std::vector<const char*>);
    };
}  // namespace Winter

#endif  // WINTER_BACKEND_H
