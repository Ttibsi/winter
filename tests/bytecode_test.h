#include <memory>
#include <vector>

#include <willow/test.h>

#include "winter/bytecode.h"
#include "winter/lexer.h"
#include "winter/parser.h"

constexpr int test_Chunk_extend([[maybe_unused]] Willow::Test* test) {
    Winter::Chunk c = Winter::Chunk();
    if (c.instructions.size()) { return 1; }

    std::vector<Winter::Bytecode> v = {
        Winter::Bytecode(Winter::Opcode::NIL),
        Winter::Bytecode(Winter::Opcode::ADD, 12.3),
    };
    c.extend(v);

    if (c.instructions.size() != 2) { return 2; }

    return 0;
}

constexpr int test_Generator_compileValue([[maybe_unused]] Willow::Test* test) {
    Winter::Generator gen = Winter::Generator(nullptr);
    Winter::ValueNode node = {5};
    auto ret = gen.compileValue(&node);
    if (!ret.has_value()) {
        test->alert(ret.error().msg);
        return 1;
    }

    if (ret.value() != Winter::Bytecode(Winter::Opcode::STORE_CONST, 5.0)) { return 2; }

    return 0;
}

constexpr int test_Generator_compileTok([[maybe_unused]] Willow::Test* test) {
    Winter::Generator gen = Winter::Generator(nullptr);

    Winter::Token tok = Winter::Token(Winter::TokenType::NIL, 0, 3);
    auto ret = gen.compileTok(&tok);
    if (!ret.has_value()) { return 1; }
    if (ret.value() != Winter::Bytecode(Winter::Opcode::NIL)) { return 2; }

    tok = Winter::Token(Winter::TokenType::PLUS, 0, 3);
    ret = gen.compileTok(&tok);
    if (!ret.has_value()) { return 3; }
    if (ret.value() != Winter::Bytecode(Winter::Opcode::ADD)) { return 4; }

    return 0;
}

constexpr int test_Generator_compileExpression([[maybe_unused]] Willow::Test* test) {
    Winter::Generator gen = Winter::Generator(nullptr);
    Winter::Token tok = Winter::Token(Winter::TokenType::PLUS, 0, 3);

    Winter::ExprNode node = Winter::ExprNode();
    node.op = &tok;
    node.lhs = std::unique_ptr<Winter::ValueNode>(new Winter::ValueNode(5));
    node.rhs = std::unique_ptr<Winter::ValueNode>(new Winter::ValueNode(3));

    auto ret = gen.compileExpression(&node);
    if (!ret.has_value()) {
        test->alert(ret.error().msg);
        return 1;
    }

    if (ret.value().size() != 3) {
        test->alert("Expr bytecode length: " + std::to_string(ret.value().size()));
        return 2;
    }

    return 0;
}

constexpr int test_Generator_compileReturn([[maybe_unused]] Willow::Test* test) {
    Winter::Generator gen = Winter::Generator(nullptr);

    Winter::ReturnNode ret_node = Winter::ReturnNode();
    ret_node.expr = std::make_unique<Winter::ExprNode>();
    ret_node.expr->lhs = std::make_unique<Winter::ValueNode>(5);

    auto ret = gen.compileReturn(&ret_node);
    if (!ret.has_value()) {
        test->alert(ret.error().msg);
        return 1;
    }

    if (ret.value().size() != 2) {
        test->alert("Expr bytecode length: " + std::to_string(ret.value().size()));
        return 2;
    }

    if (ret.value().front() != Winter::Bytecode(Winter::Opcode::STORE_CONST)) { return 3; }
    if (ret.value().back() != Winter::Bytecode(Winter::Opcode::RET)) { return 4; }

    return 0;
}

constexpr int test_Generator_compileFunc([[maybe_unused]] Willow::Test* test) {
    Winter::Generator gen = Winter::Generator(nullptr);
    Winter::FuncNode node = Winter::FuncNode();
    node.name = "f";
    node.params = {Winter::Token(Winter::TokenType::IDENT, 5, 2)};

    // Build node body for test data
    auto expr_node = std::make_unique<Winter::ExprNode>();
    expr_node->lhs = std::make_unique<Winter::ValueNode>(5);

    auto ret_node = std::make_unique<Winter::ReturnNode>();
    ret_node->expr = std::move(expr_node);  // Transfer ownership

    node.body = std::make_unique<Winter::BlockNode>();
    node.body->stmts.push_back(std::move(ret_node));

    auto ret = gen.compileFunc(&node);
    if (!ret.has_value()) {
        test->alert(ret.error().msg);
        return 1;
    }

    if (ret.value().name != "f") {
        test->alert("Chunk name" + ret.value().name);
        return 2;
    }

    if (ret.value().instructions.size() != 2) {
        test->alert("Chunk instruction len: " + std::to_string(ret.value().instructions.size()));
        return 3;
    }

    return 0;
}

constexpr int test_Generator_generate([[maybe_unused]] Willow::Test* test) {
    using namespace Winter;

    Lexer l = Lexer("func f() { return 4; }");
    result_t ret = l.tokenize();
    Parser p = Parser(&l);
    ret = p.parse_tree();
    Generator gen = Generator(std::move(p.root));
    auto gen_ret = gen.generate();

    if (!gen_ret.has_value()) {
        test->alert(gen_ret.error().msg);
        return 1;
    }

    Module m = gen_ret.value();
    if (m.name != "main") {
        test->alert("Module name: " + m.name);
        return 2;
    }

    if (m.chunks.size() != 1) { return 3; }
    const Chunk c = m.chunks.at(0);

    if (c.name != "f") {
        test->alert("Chunk name" + c.name);
        return 4;
    }

    if (c.instructions.size() != 2) {
        test->alert("Chunk instruction len: " + std::to_string(c.instructions.size()));
        return 5;
    }

    return 0;
}
