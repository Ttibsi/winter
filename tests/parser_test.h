#include <willow/test.h>

#include "helpers.h"
#include "parser.h"

constexpr int test_prefixBindingPower([[maybe_unused]] Willow::Test* test) {
    const auto p = Winter::Parser(nullptr);
    if (p.prefixBindingPower(Winter::TokenType::MINUS) != 8) {
        return 1;
    }
    if (p.prefixBindingPower(Winter::TokenType::DOT) != 0) {
        return 2;
    }
    return 0;
}

constexpr int test_infixBindingPower([[maybe_unused]] Willow::Test* test) {
    const auto p = Winter::Parser(nullptr);
    if (p.infixBindingPower(Winter::TokenType::MINUS) != 6) {
        return 1;
    }
    if (p.infixBindingPower(Winter::TokenType::EQUAL) != 1) {
        return 2;
    }
    if (p.infixBindingPower(Winter::TokenType::BANG) != 0) {
        return 3;
    }
    return 0;
}

constexpr int test_parseFunc([[maybe_unused]] Willow::Test* test) {
    auto l = std::make_unique<Winter::Lexer>("func f() { return 1; }");
    std::ignore = l->tokenize();
    auto p = Winter::Parser(std::move(l));
    auto ret = p.parseFunc();
    if (!ret.has_value()) {
        test->alert(ret.error().display());
        return 1;
    }

    const Winter::FunctionNode_ptr node = std::move(ret.value());
    if (node->name != std::string("f")) {
        test->alert("Function name: " + node->name);
        return 2;
    }

    if (node->params.size()) {
        test->alert("Params length: " + std::to_string(node->params.size()));
        return 3;
    }

    if (node->body == nullptr) {
        return 4;
    }

    return 0;
}

constexpr int test_parseBlock([[maybe_unused]] Willow::Test* test) {
    auto l = std::make_unique<Winter::Lexer>("func f() { return 1; }");
    std::ignore = l->tokenize();
    auto p = Winter::Parser(std::move(l));
    p.L->playhead = 4;

    auto ret = p.parseBlock();
    if (!ret.has_value()) {
        test->alert(ret.error().display());
        return 1;
    }

    Winter::BlockNode_ptr block = std::move(ret.value());
    if (block->stmts.size() != 1) {
        test->alert("Block statement length: " + std::to_string(block->stmts.size()));
        return 2;
    }

    return 0;
}

constexpr int test_parseStatement([[maybe_unused]] Willow::Test* test) {
    auto l = std::make_unique<Winter::Lexer>("return 1;");
    std::ignore = l->tokenize();
    auto p = Winter::Parser(std::move(l));
    auto ret = p.parseStatement();
    if (!ret.has_value()) {
        test->alert(ret.error().display());
        return 1;
    }

    const auto node = std::move(ret.value());
    if (node->getNodeType() != Winter::NodeType::ReturnNode) {
        return 2;
    }

    return 0;
}

constexpr int test_parseReturn([[maybe_unused]] Willow::Test* test) {
    auto l = std::make_unique<Winter::Lexer>("return 1;");
    std::ignore = l->tokenize();
    auto p = Winter::Parser(std::move(l));
    const auto ret = p.parseReturn();
    if (!ret.has_value()) {
        test->alert(ret.error().display());
        return 1;
    }

    if (ret.value()->expr == nullptr) {
        return 2;
    }

    return 0;
}

constexpr int test_parseExpression([[maybe_unused]] Willow::Test* test) {
    auto l = std::make_unique<Winter::Lexer>("1 + 2;");
    std::ignore = l->tokenize();
    auto p = Winter::Parser(std::move(l));
    auto ret = p.parseExpression(0);
    if (!ret.has_value()) {
        test->alert(ret.error().display());
        return 1;
    }

    const Winter::ExprNode_ptr node = std::move(ret.value());
    if (node->lhs == nullptr || node->rhs == nullptr || !node->op.has_value()) {
        test->alert("Malformed expression");
        return 2;
    }

    if (node->lhs->getNodeType() != Winter::NodeType::ValueNode) {
        test->alert("LHS is not a value node");
        return 3;
    }

    if (node->rhs->getNodeType() != Winter::NodeType::ValueNode) {
        test->alert("RHS is not a value node");
        return 4;
    }

    const Winter::ValueNode* lhs = static_cast<Winter::ValueNode*>(node->lhs.get());
    const Winter::ValueNode* rhs = static_cast<Winter::ValueNode*>(node->rhs.get());

    if (!cmpDouble(lhs->expr, 1.0)) {
        test->alert("LHS Value: " + std::to_string(lhs->expr));
        return 5;
    }

    if (!cmpDouble(rhs->expr, 2.0)) {
        test->alert("RHS Value: " + std::to_string(rhs->expr));
        return 6;
    }

    return 0;
}

constexpr int test_parseTree([[maybe_unused]] Willow::Test* test) {
    return 1;
}
