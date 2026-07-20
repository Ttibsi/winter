#include "backend.h"

#include <format>
#include <optional>
#include <string_view>
#include <variant>

#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/IRBuilder.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>

#include "../frontend/ast.h"
#include "../frontend/lexer.h"

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
    createFunction(LLVMContext& ctx, std::unique_ptr<Module>& mod, Node node, const letNode* let) {
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

    [[nodiscard]] BasicBlock*
    createBlock(LLVMContext& ctx, std::unique_ptr<Module>& mod, const letNode* let) {
        // TODO: populate twine with line number when we have that info
        // NOTE: Twine is like an assembly label, I think
        auto blk = BasicBlock::Create(ctx, Twine(), mod->getFunction(let->name));
        return blk;
    }

    [[nodiscard]] Value* compileExpression(LLVMContext& ctx, IRBuilder<>* builder, Node node) {
        exprNode* expr = std::get_if<exprNode>(&node.data);
        Value* ret = nullptr;

        if (!expr->op.has_value()) {
            // TODO: only one value in the expression
            // return something from here
        }

        switch (expr->op.value()) {
            case TokenType::plus: {
                Value* lhsVal = nullptr;
                Value* rhsVal = nullptr;

                for (std::size_t i = 0; i < node.children.size(); i++) {
                    Node child = node.children.at(i);
                    Value* activePtr = nullptr;

                    if (child.type == NodeType::numlitNode) {
                        numlitNode* numLit = std::get_if<numlitNode>(&child.data);
                        activePtr = ConstantInt::get(Type::getInt32Ty(ctx), numLit->value);
                    } else if (child.type == NodeType::exprNode) {
                        activePtr = compileExpression(ctx, builder, child);
                    }

                    (i % 2 ? rhsVal : lhsVal) = activePtr;
                }

                ret = builder->CreateAdd(lhsVal, rhsVal);
            } break;

            case TokenType::star: {
                Value* lhsVal = nullptr;
                Value* rhsVal = nullptr;

                for (std::size_t i = 0; i < node.children.size(); i++) {
                    Node child = node.children.at(i);
                    Value* activePtr = nullptr;

                    if (child.type == NodeType::numlitNode) {
                        numlitNode* numLit = std::get_if<numlitNode>(&child.data);
                        activePtr = ConstantInt::get(Type::getInt32Ty(ctx), numLit->value);
                    } else if (child.type == NodeType::exprNode) {
                        activePtr = compileExpression(ctx, builder, child);
                    }

                    (i % 2 ? rhsVal : lhsVal) = activePtr;
                }

                ret = builder->CreateMul(lhsVal, rhsVal);
            } break;
        }

        return ret;
    }

    void populateBlock(LLVMContext& ctx, BasicBlock* blk, const Node node) {
        const Node func = node.children.at(0);
        const Node body = func.children.at(0);

        IRBuilder builder(blk);

        for (Node stmt : body.children) {
            switch (stmt.type) {
                case NodeType::returnNode: {
                    // TODO: handle `return;`
                    Node retExpr = stmt.children.at(0);
                    Value* exprVal = compileExpression(ctx, &builder, retExpr);
                    builder.CreateRet(exprVal);
                } break;

                default: break;
            }
        }
    }

    [[nodiscard]] module_result_t compileModule(std::span<Node> nodes) {
        LLVMContext ctx;
        std::unique_ptr<Module> myModule = std::make_unique<Module>("Main", ctx);

        for (auto node : nodes) {
            const letNode* let = std::get_if<letNode>(&node.data);
            if (let->isFunc) {
                std::optional<Error> ret = createFunction(ctx, myModule, node, let);
                if (ret.has_value()) { return std::unexpected(ret.value()); }

                // NOTE: A function may be made up of multiple basic blocks
                // probably nested blocks, like if/else/for blocks?
                BasicBlock* blk = createBlock(ctx, myModule, let);

                populateBlock(ctx, blk, node);
            }
        }

        return myModule;
    }
}  // namespace Winter
