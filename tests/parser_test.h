#ifndef WINTER_PARSER_TEST_H
#define WINTER_PARSER_TEST_H

#include <variant>

#include <willow/willow.h>

#include "frontend/parser.h"

using namespace Winter;
using namespace std::literals::string_view_literals;

[[nodiscard]] int test_parser_check([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("let x = func() int { return 0; }"sv);
    P.consume();
    if (!P.check(TokenType::kw_let)) { return 1; }

    Parser P2("123"sv);
    P2.consume();
    if (!P2.check(TokenType::num_literal)) { return 2; }

    Parser P3("foo"sv);
    P3.consume();
    if (P3.check(TokenType::kw_let)) { return 3; }

    return 0;
}

[[nodiscard]] int test_parser_consume_void([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("let foo"sv);
    P.consume();
    if (P.current.type != TokenType::kw_let) { return 1; }
    P.consume();
    if (P.prev.type != TokenType::kw_let) { return 2; }
    if (P.current.type != TokenType::ident) { return 3; }

    return 0;
}

[[nodiscard]] int test_parser_consume_tokens([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("func("sv);
    P.consume();
    if (!P.consume({TokenType::lparen})) { return 1; }
    if (!P.check(TokenType::lparen)) { return 2; }

    Parser P2("func x"sv);
    P2.consume();
    if (P2.consume({TokenType::lparen})) { return 3; }

    return 0;
}

[[nodiscard]] int test_parser_parseArg([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] int test_parser_parseBody([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("{}"sv);
    P.consume();
    auto r = P.parseBody();
    if (!r.has_value()) { return 1; }
    if (r.value().type != NodeType::bodyNode) { return 2; }
    const auto* bd = std::get_if<bodyNode>(&r.value().data);
    if (bd == nullptr || bd->childCount != 0) { return 3; }

    Parser P2("{ return 1; }"sv);
    P2.consume();
    auto r2 = P2.parseBody();
    if (!r2.has_value()) { return 4; }
    const auto* bd2 = std::get_if<bodyNode>(&r2.value().data);
    if (bd2 == nullptr || bd2->childCount != 1) { return 5; }
    if (r2.value().children[0].type != NodeType::returnNode) { return 6; }

    return 0;
}

[[nodiscard]] int test_parser_parseCallOrVariable([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] int test_parser_parseExpr([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("42;"sv);
    P.consume();
    auto r = P.parseExpr(0);
    if (!r.has_value()) { return 1; }
    const auto* nl = std::get_if<numlitNode>(&r.value().data);
    if (nl == nullptr || nl->value != 42) { return 2; }

    Parser P2("1+2;"sv);
    P2.consume();
    auto r2 = P2.parseExpr(0);
    if (!r2.has_value()) { return 3; }
    const auto* ex = std::get_if<exprNode>(&r2.value().data);
    if (ex == nullptr || ex->childCount != 2 || !ex->op.has_value() ||
        ex->op.value() != TokenType::plus) {
        return 4;
    }
    const auto* lhs = std::get_if<numlitNode>(&r2.value().children[0].data);
    const auto* rhs = std::get_if<numlitNode>(&r2.value().children[1].data);
    if (lhs == nullptr || rhs == nullptr || lhs->value != 1 || rhs->value != 2) { return 5; }

    return 0;
}

[[nodiscard]] int test_parser_parseFunc([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("func() void {}"sv);
    P.consume();
    auto r = P.parseFunc();
    if (!r.has_value()) { return 1; }
    const auto* fn = std::get_if<funcNode>(&r.value().data);
    if (fn == nullptr || fn->parameters.size() != 0 || fn->retType != "void") { return 2; }
    if (r.value().children.size() != 1 || r.value().children[0].type != NodeType::bodyNode) {
        return 3;
    }

    Parser P2("func(a: int, b: int) void {}"sv);
    P2.consume();
    auto r2 = P2.parseFunc();
    if (!r2.has_value()) { return 4; }
    const auto* fn2 = std::get_if<funcNode>(&r2.value().data);
    if (fn2 == nullptr || fn2->parameters.size() != 2) { return 5; }

    return 0;
}

[[nodiscard]] int test_parser_parseFuncCall([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] int test_parser_parseLet([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("let main = func() int { return 0; }"sv);
    P.consume();
    auto r = P.parseLet();
    if (!r.has_value()) { return 1; }
    const auto* ln = std::get_if<letNode>(&r.value().data);
    if (ln == nullptr || ln->name != "main" || !ln->isFunc) { return 2; }
    if (r.value().children.size() != 1 || r.value().children[0].type != NodeType::funcNode) {
        return 3;
    }

    return 0;
}

[[nodiscard]] int test_parser_parseNumLit([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("123"sv);
    P.consume();
    auto r = P.parseNumLit();
    if (!r.has_value()) { return 1; }
    const auto* nl = std::get_if<numlitNode>(&r.value().data);
    if (nl == nullptr || nl->value != 123) { return 2; }

    Parser P2("9"sv);
    P2.consume();
    const auto nr = P2.parseNumLit();
    if (!nr.has_value()) { return 3; }
    if (!P2.check(TokenType::num_literal)) { return 4; }

    return 0;
}

[[nodiscard]] int test_parser_parseParam([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("count: int"sv);
    P.consume();
    auto r = P.parseParam();
    if (!r.has_value()) { return 1; }
    const auto* pm = std::get_if<paramNode>(&r.value().data);
    if (pm == nullptr || pm->name != "count" || pm->type != ":") { return 2; }

    return 0;
}

[[nodiscard]] int test_parser_parseReturn([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("return 42;"sv);
    P.consume();
    auto r = P.parseReturn();
    if (!r.has_value()) { return 1; }
    if (r.value().type != NodeType::returnNode) { return 2; }
    if (r.value().children.size() != 1) { return 3; }
    const auto* nl = std::get_if<numlitNode>(&r.value().children[0].data);
    if (nl == nullptr || nl->value != 42) { return 4; }

    return 0;
}

[[nodiscard]] int test_parser_parseVariable([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] int test_parser_operatorCall([[maybe_unused]] Willow::Test* test) noexcept {
    Parser P("let x = func() int { return 0; }"sv);
    auto r = P();
    if (!r.has_value()) { return 1; }
    if (r.value().size() != 1) { return 2; }
    const auto* ln = std::get_if<letNode>(&r.value()[0].data);
    if (ln == nullptr || ln->name != "x") { return 3; }

    return 0;
}

#endif  // WINTER_PARSER_TEST_H
