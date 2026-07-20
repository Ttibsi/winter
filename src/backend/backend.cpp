#include "backend.h"

#include <format>
#include <optional>
#include <string_view>
#include <variant>

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>

#include "../frontend/ast.h"

using namespace llvm;

namespace Winter {
    [[nodiscard]] std::expected<Type*, Error> getType(LLVMContext& ctx, std::string_view type_str) {
        if (type_str == "i32") {
            Type* int32Type = Type::getInt32Ty(ctx);
            return int32Type;
        }

        return std::unexpected(
            Error(ErrType::Generator, std::format("Type not found: {}", type_str)));
    }

    [[nodiscard]] std::optional<Error>
    createFunction(LLVMContext& ctx, Module* mod, Node node, const letNode* let) {
        const funcNode* func = std::get_if<funcNode>(&node.children.at(0).data);

        std::expected<Type*, Error> retType = getType(ctx, func->retType);
        if (!retType.has_value()) { return retType.error(); }

        std::vector<Type*> paramList = {};
        for (auto param : func->parameters) {
            const paramNode* p = std::get_if<paramNode>(&param.data);
            std::expected<Type*, Error> paramType = getType(ctx, p->type);
            if (!paramType.has_value()) { return paramType.error(); }
            paramList.push_back(paramType.value());
        }

        auto fType = FunctionType::get(retType.value(), ArrayRef(paramList), false);
        mod->getOrInsertFunction(let->name, fType);
        return {};
    }

    [[nodiscard]] BasicBlock* createBlock(LLVMContext& ctx, Module* mod, const letNode* let) {
        // TODO: populate twine with line number when we have that info
        // NOTE: Twine is like an assembly label, I think
        auto blk = BasicBlock::Create(ctx, Twine(), mod->getFunction(let->name));
        return blk;
    }

    void populateBlock(LLVMContext& ctx, BasicBlock* blk, const Node node) {
        const Node func = node.children.at(0);
        const Node body = func.children.at(0);

        for (auto stmt : body.children) {}
    }

    [[nodiscard]] std::optional<Error> compileModule(std::span<Node> nodes) {
        LLVMContext ctx;
        Module myModule("Main", ctx);

        for (auto node : nodes) {
            const letNode* let = std::get_if<letNode>(&node.data);
            if (let->isFunc) {
                std::optional<Error> ret = createFunction(ctx, &myModule, node, let);
                if (ret.has_value()) { return ret.value(); }

                // NOTE: A function may be made up of multiple basic blocks
                // probably nested blocks, like if/else/for blocks?
                BasicBlock* blk = createBlock(ctx, &myModule, let);

                populateBlock(ctx, blk, node);
            }
        }

        return {};
    }
}  // namespace Winter
