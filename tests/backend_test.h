#ifndef WINTER_BACKEND_TEST_H
#define WINTER_BACKEND_TEST_H

#include <optional>

#include <llvm/MC/TargetRegistry.h>
#include <willow/willow.h>

#include "backend/backend.h"
#include "frontend/parser.h"
#include "llvm/ADT/StringRef.h"

using namespace Winter;

[[nodiscard]] constexpr int test_getType([[maybe_unused]] Willow::Test* test) noexcept {
    Backend B = Backend("test");
    const auto t = B.getType("i32");

    if (!t.has_value()) { return 1; }
    if (!t.value()->isIntegerTy(32)) { return 2; }

    return 0;
}

[[nodiscard]] constexpr int test_getTarget([[maybe_unused]] Willow::Test* test) noexcept {
    Backend B = Backend("test");
    const auto t = B.getTarget();

    if (!t.has_value()) { return 1; }
    if (t.value()->getName() != llvm::StringRef("x86-64")) {
        test->alert(t.value()->getName());
        return 2;
    }

    return 0;
}

[[nodiscard]] constexpr int test_createFunction([[maybe_unused]] Willow::Test* test) noexcept {
    // We need to get a letNode
    Parser P("let x = func() i32 { return 0; }"sv);
    P.consume();
    Node_Result maybe_let = P.parseLet(false);
    if (!maybe_let.has_value()) { return 1; }
    const letNode* let = std::get_if<letNode>(&maybe_let.value().data);

    Backend B = Backend("test");
    module_ptr_t mod = std::make_unique<Module>("test_createFunction", B.ctx);
    B.currentNode = maybe_let.value();
    std::optional<Winter::Error> ret = B.createFunction(mod, let);
    if (ret.has_value()) {
        test->alert(ret.value().msg);
        return 2;
    }

    return 0;
}

[[nodiscard]] constexpr int test_createBlock([[maybe_unused]] Willow::Test* test) noexcept {
    Backend B = Backend("test");
    module_ptr_t mod = std::make_unique<Module>("test_createBlock", B.ctx);
    const letNode let = letNode("foo", true, false);
    BasicBlock* blk = B.createBlock(mod, &let);
    if (blk == nullptr) { return 1; }
    return 0;
}

[[nodiscard]] constexpr int test_compileExpression([[maybe_unused]] Willow::Test* test) noexcept {
    // // We need to get a letNode
    Parser P("let x = func() i32 { return 34 + 35; }"sv);
    P.consume();
    Node_Result maybe_let = P.parseLet(false);
    if (!maybe_let.has_value()) { return 1; }
    const letNode* let = std::get_if<letNode>(&maybe_let.value().data);

    // currentNode needs to be an expression
    // let > function > body > return > expr
    const Node body = maybe_let.value().children.at(0).children.at(0);
    const Node expr = body.children.at(0).children.at(0);

    Backend B = Backend("test");
    module_ptr_t mod = std::make_unique<Module>("test_compileExpression", B.ctx);
    B.currentNode = expr;
    IRBuilder builder(B.createBlock(mod, let));

    Value* value = B.compileExpression(&builder);

    // if (value->getName() != llvm::StringRef("test_compileExpression")) {
    //     test->alert(std::string(value->getName()));
    //     return 2;
    // }

    return 0;
}

[[nodiscard]] constexpr int test_populateBlock([[maybe_unused]] Willow::Test* test) noexcept {
    // // We need to get a letNode
    Parser P("let x = func() i32 { return 34 + 35; }"sv);
    P.consume();
    Node_Result maybe_let = P.parseLet(false);
    if (!maybe_let.has_value()) { return 1; }
    const letNode* let = std::get_if<letNode>(&maybe_let.value().data);

    Backend B = Backend("test");
    module_ptr_t mod = std::make_unique<Module>("test_createBlock", B.ctx);
    BasicBlock* blk = B.createBlock(mod, let);
    if (blk == nullptr) { return 2; }

    B.currentNode = maybe_let.value();
    B.populateBlock(blk);

    return 0;
}

[[nodiscard]] constexpr int test_compileModule([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_outputObjectFile([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

#endif  // WINTER_BACKEND_TEST_H
