#include <willow/test.h>

#include "winter/parser.h"

constexpr int test_prefixBindingPower([[maybe_unused]] Willow::Test* test) {
    const auto p = Winter::Parser(nullptr);
    if (p.prefixBindingPower(Winter::TokenType::MINUS) != 8) { return 1; }
    if (p.prefixBindingPower(Winter::TokenType::DOT) != 0) { return 2; }
    return 0;
}

constexpr int test_infixBindingPower([[maybe_unused]] Willow::Test* test) {
    const auto p = Winter::Parser(nullptr);
    if (p.infixBindingPower(Winter::TokenType::MINUS) != 6) { return 1; }
    if (p.infixBindingPower(Winter::TokenType::EQUAL) != 1) { return 2; }
    if (p.infixBindingPower(Winter::TokenType::BANG) != 0) { return 3; }
    return 0;
}

constexpr int test_parseBlock([[maybe_unused]] Willow::Test* test) {
    auto L = Winter::Lexer("{ return 4; }");
    std::ignore = L.tokenize();
    auto p = Winter::Parser(&L);
    auto ret = p.parseBlock();

    if (!ret.has_value()) { return 1; }
    if (ret.value() == nullptr) { return 2; }

    std::unique_ptr<Winter::BlockNode> node = std::move(ret.value());
    if (node->stmts.size() != 1) { return 3; }

    return 0;
}

constexpr int test_parseExpression([[maybe_unused]] Willow::Test* test) {
    auto L = Winter::Lexer("4 + 3;");
    std::ignore = L.tokenize();
    auto p = Winter::Parser(&L);
    auto ret = p.parseExpression(0);

    if (!ret.has_value()) { return 1; }
    if (ret.value() == nullptr) { return 2; }

    std::unique_ptr<Winter::ExprNode> node = std::move(ret.value());
    if (node->op == nullptr) { return 3; }
    if (node->op->type != Winter::TokenType::PLUS) { return 4; }

    if (node->lhs == nullptr) { return 5; }
    if (node->rhs == nullptr) { return 6; }

    return 0;
}

constexpr int test_parseFunc([[maybe_unused]] Willow::Test* test) {
    auto L = Winter::Lexer("func foo(argc, argv) { return 4; }");
    std::ignore = L.tokenize();
    auto p = Winter::Parser(&L);
    auto ret = p.parseFunc();

    if (!ret.has_value()) { return 1; }
    if (ret.value() == nullptr) { return 2; }

    std::unique_ptr<Winter::FuncNode> node = std::move(ret.value());
    if (node->name != std::string("foo")) { return 3; }
    if (node->params.size() != 2) { return 4; }

    if (node->params.at(0).type != Winter::TokenType::IDENT) { return 5; }
    if (node->params.at(0).len != 4) { return 6; }
    if (node->body == nullptr) { return 7; }

    return 0;
}

constexpr int test_parseReturn([[maybe_unused]] Willow::Test* test) {
    auto L = Winter::Lexer("return 4;");
    std::ignore = L.tokenize();
    auto p = Winter::Parser(&L);
    auto ret = p.parseReturn();

    if (!ret.has_value()) { return 1; }
    if (ret.value() == nullptr) { return 2; }

    std::unique_ptr<Winter::ReturnNode> node = std::move(ret.value());
    if (node->expr == nullptr) { return 3; }

    return 0;
}

constexpr int test_parseStatement([[maybe_unused]] Willow::Test* test) {
    auto L = Winter::Lexer("return 4;");
    std::ignore = L.tokenize();
    auto p = Winter::Parser(&L);
    auto ret = p.parseStatement();

    if (!ret.has_value()) { return 1; }
    if (ret.value() == nullptr) { return 2; }

    Winter::ReturnNode* node = static_cast<Winter::ReturnNode*>(ret.value().get());
    if (node->expr == nullptr) { return 3; }

    return 0;
}

constexpr int test_parse_tree([[maybe_unused]] Willow::Test* test) {
    auto L = Winter::Lexer("func foo(argc, argv) { return 4; }");
    std::ignore = L.tokenize();
    auto p = Winter::Parser(&L);
    auto ret = p.parse_tree();

    if (!ret.has_value()) { return 1; }
    if (p.root == nullptr) { return 2; }
    if (p.root->children.size() != 1) {
        test->alert("Children len: " + std::to_string(p.root->children.size()));
        return 3;
    }

    Winter::FuncNode* func = static_cast<Winter::FuncNode*>(p.root->children.at(0).get());
    if (func->name != "foo") { return 4; }
    if (func->body == nullptr) { return 5; }

    return 0;
}
