#include "bytecode.h"

namespace Winter {
    [[nodiscard]] expected_bytecode_t Generator::compileValue(const ValueNode* node) {
        return Err::TODO();
    }

    [[nodiscard]] Bytecode Generator::compileTok(const Token* op) {
        switch (op->type) {
            case TokenType::MINUS:
                return Bytecode(Opcode::SUB);
                break;
            case TokenType::PLUS:
                return Bytecode(Opcode::ADD);
                break;
            case TokenType::STAR:
                return Bytecode(Opcode::MUL);
                break;
            case TokenType::SLASH:
                return Bytecode(Opcode::DIV);
                break;
            default:
                return Bytecode(Opcode::NIL);
        }
    }

    [[nodiscard]] expected_bytecode_t Generator::compileExpression(const ExprNode* node) {
        std::vector<Bytecode> bytecode = {};

        auto dispatch = [this, &bytecode](std::unique_ptr<ASTNode> inner) -> expected_bytecode_t {
            if (inner == nullptr) { return {}; }

            switch (inner->getNodeType()) {
                case NodeType::ValueNode: {
                    ValueNode* val_node = static_cast<ValueNode*>(inner.get());
                    auto ret = compileValue(val_node);
                    if (!ret.has_value()) { return std::unexpected(ret.error()); }
                    bytecode.insert(bytecode.end(), ret.value().begin(), ret.value().end());
                } break;

                default:
                    return std::unexpected(
                        Err(ErrType::BytecodeGenerationError, "Incorrect node type"));
            }

            std::unreachable();
        };

        auto elem_bc = dispatch(std::move(node->lhs));
        if (!elem_bc.has_value()) { return std::unexpected(elem_bc.error()); }
        bytecode.insert(bytecode.end(), elem_bc.begin(), elem_bc.end());

        elem_bc = dispatch(rhs);
        if (!elem_bc.has_value()) { return std::unexpected(elem_bc.error()); }
        bytecode.insert(bytecode.end(), elem_bc.begin(), elem_bc.end());

        bytecode.push_back(compileTok(op));
        return bytecode;
    }

    [[nodiscard]] expected_bytecode_t Generator::compileReturn(const ReturnNode* node) {
        const ExprNode* expr = static_cast<ExprNode*>(node->expr.get());
        expected_bytecode_t expr_bc = compileExpression(expr);
        if (!expr_bc.has_value()) { return std::unexpected(expr_bc.error()); }

        expr_bc.value().push_back(Bytecode(Opcode::RET));
        return expr_bc.value();
    }

    [[nodiscard]] expected_chunk_t Generator::compileFunc(const FuncNode* node) {
        Chunk chunk = Chunk(node->name);

        for (auto&& elem : node->body->stmts) {
            switch (elem->getNodeType()) {
                case NodeType::ReturnNode: {
                    ReturnNode* retNode = static_cast<ReturnNode*>(elem.get());
                    expected_bytecode_t bc = compileReturn(retNode);
                    if (!bc.has_value()) { return std::unexpected(bc.error()); }
                    chunk.extend(bc.value());
                } break;
            }
        }

        return chunk;
    }

    [[nodiscard]] std::expected<Module, Err> Generator::generate() {
        Module mod = Module("main");

        if (ast_node->getNodeType() != NodeType::RootNode) {
            return std::unexpected(
                Err(ErrType::BytecodeGenerationError, "Node provided not Root node"));
        }

        for (auto&& child : ast_node->children) {
            switch (child->getNodeType()) {
                case (NodeType::FuncNode):
                    expected_chunk_t func = compileFunc(static_cast<FuncNode*>(child.get()));
                    if (!func.has_value()) { return std::unexpected(func.error()); }
                    mod.chunks.push_back(func.value());
                    break;
            }
        };

        return mod;
    }

}  // namespace Winter
