#include "backend.h"

#include <format>
#include <string_view>
#include <variant>

#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include "../frontend/ast.h"
#include "llvm/ADT/ArrayRef.h"
#include "llvm/IR/BasicBlock.h"

namespace Winter {
    [[nodiscard]] std::expected<llvm::Type*, Error> getType(
        llvm::LLVMContext& ctx,
        std::string_view type_str) {
        if (type_str == "i32") {
            llvm::Type* int32Type = llvm::Type::getInt32Ty(ctx);
            return int32Type;
        }

        return std::unexpected(
            Error(ErrType::Generator, std::format("Type not found: {}", type_str)));
    }

    [[nodiscard]] std::expected<void, Error>
    createFunction(llvm::LLVMContext& ctx, llvm::Module* mod, Node node, const letNode* let) {
        const funcNode* func = std::get_if<funcNode>(&node.children.at(0).data);

        std::expected<llvm::Type*, Error> retType = getType(ctx, func->retType);
        if (!retType.has_value()) { return std::unexpected(retType.error()); }

        std::vector<llvm::Type*> paramList = {};
        for (auto param : func->parameters) {
            const paramNode* p = std::get_if<paramNode>(&param.data);
            std::expected<llvm::Type*, Error> paramType = getType(ctx, p->type);
            if (!paramType.has_value()) { return std::unexpected(paramType.error()); }
            paramList.push_back(paramType.value());
        }

        auto fType = llvm::FunctionType::get(retType.value(), llvm::ArrayRef(paramList), false);
        mod->getOrInsertFunction(let->name, fType);
        return {};
    }

    // static BasicBlock * 	Create (LLVMContext &Context, const Twine &Name="", Function
    // *Parent=nullptr, BasicBlock *InsertBefore=nullptr)
    void populateWithBlocks(llvm::LLVMContext& ctx, llvm::Module* mod, Node node) {
        const Node func = node.children.at(0);
        const Node body = func.children.at(0);
        // const bodyNode* body = std::get_if<bodyNode>(&func.children.at(0).data);

        for (auto stmt : body.children) {}
    }

    [[nodiscard]] std::expected<void, Error> compileModule(std::span<Node> nodes) {
        llvm::LLVMContext ctx;
        llvm::Module myModule("Main", ctx);

        for (auto node : nodes) {
            const letNode* let = std::get_if<letNode>(&node.data);
            if (let->isFunc) {
                std::expected<void, Error> ret = createFunction(ctx, &myModule, node, let);
                if (!ret.has_value()) { return std::unexpected(ret.error()); }

                populateWithBlocks(ctx, &myModule, node);
            }
        }

        return {};
    }
}  // namespace Winter
