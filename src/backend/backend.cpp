#include "backend.h"

#include <format>
#include <optional>
#include <print>
#include <string>
#include <variant>

#include <lld/Common/Driver.h>
#include <llvm/ADT/ArrayRef.h>
#include <llvm/ADT/Twine.h>
#include <llvm/CodeGen/CommandFlags.h>
#include <llvm/IR/BasicBlock.h>
#include <llvm/IR/Constants.h>
#include <llvm/IR/DerivedTypes.h>
#include <llvm/IR/Instructions.h>
#include <llvm/IR/LegacyPassManager.h>
#include <llvm/IR/Module.h>
#include <llvm/IR/Type.h>
#include <llvm/IR/Value.h>
#include <llvm/MC/TargetRegistry.h>
#include <llvm/Support/FileSystem.h>
#include <llvm/Support/TargetSelect.h>
#include <llvm/Support/ToolOutputFile.h>
#include <llvm/Target/TargetMachine.h>
#include <llvm/TargetParser/Host.h>

#include "../frontend/ast.h"
#include "../frontend/lexer.h"

LLD_HAS_DRIVER(elf);

namespace Winter {
    [[nodiscard]] std::expected<Type*, Error> Backend::getType(std::string_view type_str) {
        if (type_str == "i32") {
            Type* int32Type = Type::getInt32Ty(ctx);
            return int32Type;
        }

        return std::unexpected(
            Error(ErrType::Generator, std::format("Type not found: '{}'", type_str)));
    }

    // based on llc code:
    // https://github.com/llvm/llvm-project/blob/main/llvm/tools/llc/llc.cpp#L607C1-L607C77
    // LICENSE: https://github.com/llvm/llvm-project/blob/main/LICENSE.TXT
    [[nodiscard]] std::expected<const Target*, Error> Backend::getTarget() {
        targetTriple = Triple(sys::getDefaultTargetTriple());

        // These are the registries that lookupTarget queryies
        InitializeAllTargetInfos();
        InitializeAllTargets();
        InitializeAllTargetMCs();
        InitializeAllAsmParsers();
        InitializeAllAsmPrinters();

        std::string ErrStr;
        const Target* target = TargetRegistry::lookupTarget(targetTriple.value(), ErrStr);
        if (target == nullptr) {
            return std::unexpected(
                Error(ErrType::Generator, std::format("Target could not be found: {}", ErrStr)));
        }

        return target;
    }

    [[nodiscard]] std::optional<Error> Backend::createFunction(
        module_ptr_t& mod,
        const letNode* let) {
        const funcNode* func = std::get_if<funcNode>(&currentNode.children.at(0).data);

        std::expected<Type*, Error> retType = getType(func->retType);
        if (!retType.has_value()) { return retType.error(); }

        std::vector<Type*> paramList = {};
        for (auto param : func->parameters) {
            const paramNode* p = std::get_if<paramNode>(&param.data);
            std::expected<Type*, Error> paramType = getType(p->type);
            if (!paramType.has_value()) { return paramType.error(); }
            paramList.push_back(paramType.value());
        }

        auto fType = FunctionType::get(retType.value(), ArrayRef(paramList), false);
        mod->getOrInsertFunction(let->name, fType);
        return {};
    }

    [[nodiscard]] BasicBlock* Backend::createBlock(module_ptr_t& mod, const letNode* let) {
        // TODO: populate twine with line number when we have that info
        // NOTE: Twine is like an assembly label, I think
        auto blk = BasicBlock::Create(ctx, Twine(), mod->getFunction(let->name));
        return blk;
    }

    [[nodiscard]] Value* Backend::compileExpression(IRBuilder<>* builder) {
        exprNode* expr = std::get_if<exprNode>(&currentNode.data);
        Value* ret = nullptr;

        if (!expr->op.has_value()) {
            // TODO: only one value in the expression
            // return something from here
        }

        switch (expr->op.value()) {
            case TokenType::plus: {
                Value* lhsVal = nullptr;
                Value* rhsVal = nullptr;

                for (std::size_t i = 0; i < currentNode.children.size(); i++) {
                    Node child = currentNode.children.at(i);
                    Value* activePtr = nullptr;

                    if (child.type == NodeType::numlitNode) {
                        numlitNode* numLit = std::get_if<numlitNode>(&child.data);
                        activePtr = ConstantInt::get(Type::getInt32Ty(ctx), numLit->value);
                    } else if (child.type == NodeType::exprNode) {
                        currentNode = child;
                        activePtr = compileExpression(builder);
                    }

                    (i % 2 ? rhsVal : lhsVal) = activePtr;
                }

                ret = builder->CreateAdd(lhsVal, rhsVal);
            } break;

            case TokenType::star: {
                Value* lhsVal = nullptr;
                Value* rhsVal = nullptr;

                for (std::size_t i = 0; i < currentNode.children.size(); i++) {
                    Node child = currentNode.children.at(i);
                    Value* activePtr = nullptr;

                    if (child.type == NodeType::numlitNode) {
                        numlitNode* numLit = std::get_if<numlitNode>(&child.data);
                        activePtr = ConstantInt::get(Type::getInt32Ty(ctx), numLit->value);
                    } else if (child.type == NodeType::exprNode) {
                        currentNode = child;
                        activePtr = compileExpression(builder);
                    }

                    (i % 2 ? rhsVal : lhsVal) = activePtr;
                }

                ret = builder->CreateMul(lhsVal, rhsVal);
            } break;

            default: break;
        }

        return ret;
    }

    void Backend::populateBlock(BasicBlock* blk) {
        const Node func = currentNode.children.at(0);
        const Node body = func.children.at(0);

        IRBuilder builder(blk);

        for (Node stmt : body.children) {
            switch (stmt.type) {
                case NodeType::returnNode: {
                    // TODO: handle `return;`
                    currentNode = stmt.children.at(0);
                    Value* exprVal = compileExpression(&builder);
                    builder.CreateRet(exprVal);
                } break;

                default: break;
            }
        }
    }

    [[nodiscard]] module_result_t Backend::compileModule(std::span<Node> nodes) {
        module_ptr_t myModule = std::make_unique<Module>("Main", ctx);

        for (auto node : nodes) {
            const letNode* let = std::get_if<letNode>(&node.data);
            if (let->isFunc) {
                currentNode = node;
                std::optional<Error> ret = createFunction(myModule, let);
                if (ret.has_value()) { return std::unexpected(ret.value()); }

                // NOTE: A function may be made up of multiple basic blocks
                // probably nested blocks in source code, like if/else/for blocks?
                BasicBlock* blk = createBlock(myModule, let);

                populateBlock(blk);
            }
        }

        return myModule;
    }

    void Backend::display_module(module_ptr_t& mod) const {
        std::println("=== BACKEND ===");
        mod->print(llvm::errs(), nullptr);
    }

    [[nodiscard]] std::expected<std::string, Error> Backend::outputObjectFile(module_ptr_t& mod) {
        std::expected<const Target*, Error> target = getTarget();
        if (!target.has_value()) { return std::unexpected(target.error()); }

        TargetOptions opts;
        TargetMachine* targetMachine = target.value()->createTargetMachine(
            targetTriple.value(), "generic", "", opts, codegen::getExplicitRelocModel());

        mod->setDataLayout(targetMachine->createDataLayout());
        mod->setTargetTriple(targetTriple.value());

        // TODO: replace the static method we stole at the top of this file
        // with these lines -- but use the input filename transformation
        std::string Filename = "output.o";
        std::error_code EC;
        raw_fd_ostream dest(Filename, EC, sys::fs::OF_None);

        legacy::PassManager PM;
        auto fileType = CodeGenFileType::ObjectFile;
        if (targetMachine->addPassesToEmitFile(PM, dest, nullptr, fileType)) {
            return std::unexpected(
                Error(ErrType::Generator, "Unknown error with addPassesToEmitFile"));
        }

        PM.run(*mod);
        dest.flush();

        return Filename;
    }

    [[nodiscard]] std::optional<Error> Backend::linkModules(std::vector<const char*> files) {
        auto args = llvm::ArrayRef(files);
        lld::Result result =
            lld::lldMain(args, llvm::outs(), llvm::errs(), {{lld::Flavor::Gnu, &lld::elf::link}});
        if (result.retCode) {
            return Error(ErrType::Generator, std::format("lld retcode: {}", result.retCode));
        }
        return {};
    }

}  // namespace Winter
