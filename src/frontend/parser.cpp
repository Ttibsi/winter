#include "parser.h"

#include <algorithm>
#include <format>
#include <print>

namespace Winter {
    [[nodiscard]] bool Parser::check(const TokenType& type) const noexcept {
        return current.type == type;
    }

    void Parser::consume() noexcept {
        prev = current;
        auto ret = L();
        if (!ret.has_value()) {
            current = Token::tombstone();
        } else {
            current = ret.value();
        }
    }

    [[nodiscard]] bool Parser::consume(std::initializer_list<TokenType> tokens) noexcept {
        consume();

        for (auto&& tok : tokens) {
            if (check(tok)) { return true; }
        }

        return false;
    }

    [[nodiscard]] Node_Result Parser::parseArg() noexcept {
        std::vector<TokenType> valid_types = {
            TokenType::num_literal,
            TokenType::char_literal,
            TokenType::str_literal,
            TokenType::ident,
        };

        if (std::none_of(valid_types.begin(), valid_types.end(), [this](const TokenType& t) {
                return check(t);
            })) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: invalid arg token"));
        }

        if (check(TokenType::str_literal) || check(TokenType::ident)) {
            return Node(
                NodeType::argNode, argNode(current.toString(&L), std::nullopt, std::nullopt));
        }

        if (check(TokenType::num_literal)) {
            return Node(NodeType::argNode, argNode(std::nullopt, current.toNum(&L), std::nullopt));
        }

        if (check(TokenType::char_literal)) {
            return Node(NodeType::argNode, argNode(std::nullopt, std::nullopt, current.toChar(&L)));
        }

        return std::unexpected(Error(ErrType::Parser, "Unknown arg type"));
    }

    [[nodiscard]] Node_Result Parser::parseBody() noexcept {
        if (!check(TokenType::lbrace)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected lbrace"));
        }

        consume();

        std::vector<Node> children = {};
        while (!check(TokenType::rbrace)) {
            if (check(TokenType::kw_return)) {
                Node_Result maybe_return = parseReturn();
                if (!maybe_return.has_value()) { return std::unexpected(maybe_return.error()); }
                children.push_back(maybe_return.value());

            } else if (check(TokenType::ident)) {
                Node_Result maybe_return = parseCallOrVariable();
                if (!maybe_return.has_value()) { return std::unexpected(maybe_return.error()); }
                children.push_back(maybe_return.value());

            } else if (check(TokenType::kw_if)) {
                Node_Result maybe_return = parseIf();
                if (!maybe_return.has_value()) { return std::unexpected(maybe_return.error()); }
                children.push_back(maybe_return.value());

            } else if (check(TokenType::kw_let)) {
                Node_Result maybe_return = parseLet();
                if (!maybe_return.has_value()) { return std::unexpected(maybe_return.error()); }
                children.push_back(maybe_return.value());
                consume();  // consume ';'

            } else {
                return std::unexpected(Error(ErrType::Parser, "Token not known in body"));
            }
        }

        consume();  // consume '}'
        return Node(NodeType::bodyNode, bodyNode(static_cast<int>(children.size())), children);
    }

    [[nodiscard]] Node_Result Parser::parseCallOrVariable() noexcept {
        if (!check({TokenType::ident})) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected ident"));
        }

        consume();
        if (check(TokenType::lparen)) { return parseFuncCall(); }
        return parseVariable();
    }

    [[nodiscard]] Node_Result Parser::parseExpr(std::size_t min_bp) noexcept {
        Node lhs = Node::tombstone();
        switch (current.type) {
            case TokenType::num_literal: {
                Node_Result lhs_ret = parseNumLit();
                if (!lhs_ret.has_value()) { return std::unexpected(lhs_ret.error()); }
                lhs = lhs_ret.value();
                consume();
            } break;

            case TokenType::str_literal: {
                Node_Result lhs_ret = parseStrLit();
                if (!lhs_ret.has_value()) { return std::unexpected(lhs_ret.error()); }
                lhs = lhs_ret.value();
                consume();
            } break;

            case TokenType::lparen: {
                consume();
                Node_Result lhs_ret = parseExpr(0);
                if (!lhs_ret.has_value()) { return std::unexpected(lhs_ret.error()); }
                lhs = lhs_ret.value();

                if (!check(TokenType::rparen)) {
                    return std::unexpected(Error(ErrType::Parser, "Expected ')'"));
                }
                consume();  // Consume ')'
            } break;

            case TokenType::kw_true: {
                lhs = Node(NodeType::boolNode, boolNode(true));
                consume();
            } break;

            case TokenType::kw_false: {
                lhs = Node(NodeType::boolNode, boolNode(false));
                consume();
            } break;

            case TokenType::semicolon: break;
            default:
                return std::unexpected(Error(ErrType::Parser, "Unexpected token in pratt parsing"));
        };

        while (true) {
            if (check(TokenType::semicolon)) { return lhs; }
            if (check(TokenType::rparen)) { return lhs; }

            const TokenType op = current.type;
            const auto bp = infixBindingPower.find(op);
            if (bp == infixBindingPower.end()) {
                return std::unexpected(
                    Error(ErrType::Parser, std::format("No bp found for op: {}", op)));
            }

            if (bp->second < min_bp) { break; }
            consume();

            Node_Result rhs = parseExpr(bp->second);
            if (!rhs.has_value()) { return std::unexpected(rhs.error()); }

            lhs = Node(NodeType::exprNode, exprNode(2, op), {lhs, rhs.value()});
        }

        return lhs;
    }

    [[nodiscard]] Node_Result Parser::parseFunc() noexcept {
        if (!check(TokenType::kw_func)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected func"));
        }

        if (!consume({TokenType::lparen})) {
            return std::unexpected(
                Error(ErrType::Parser, "Unexpected token: func arguments not specified"));
        }

        consume();  // Consume the lparen we've just moved to

        // Contents
        std::vector<Node> parameters = {};
        std::string retType;

        while (!check(TokenType::rparen)) {
            auto param = parseParam();
            if (!param.has_value()) { return std::unexpected(param.error()); }

            parameters.push_back(param.value());
            // Move forward to next token. If it's a comma, move ahead again
            if (consume({TokenType::comma})) { consume(); }
        }

        if (!consume({TokenType::ident})) {
            return std::unexpected(Error(ErrType::Parser, "function return type not found"));
        }

        retType = current.toString(&L);

        if (!consume({TokenType::lbrace})) {
            return std::unexpected(Error(ErrType::Parser, "function body not not found"));
        }

        Node_Result expected_body = parseBody();
        if (!expected_body.has_value()) { return std::unexpected(expected_body.error()); }

        return Node(NodeType::funcNode, funcNode(parameters, retType), {expected_body.value()});
    }

    [[nodiscard]] Node_Result Parser::parseFuncCall() noexcept {
        if (!check({TokenType::lparen})) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected lparen"));
        }

        // NOTE: the function name token is at `prev`
        std::string funcName = prev.toString(&L);
        std::vector<Node> args = {};

        consume();

        while (!check(TokenType::rparen)) {
            auto arg = parseArg();
            if (!arg.has_value()) { return std::unexpected(arg.error()); }

            args.push_back(arg.value());
            // Move forward to next token. If it's a comma, move ahead again
            if (consume({TokenType::comma})) { consume(); }
        }

        consume();  // consume rparen
        consume();  // consume semicolon
        return Node(NodeType::callNode, funcCallNode(funcName), args);
    }

    [[nodiscard]] Node_Result Parser::parseIf() noexcept {
        if (!check(TokenType::kw_if)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_if"));
        }

        if (!consume({TokenType::lparen})) {
            return std::unexpected(
                Error(ErrType::Parser, "If statement not followed by expression"));
        }

        consume();
        Node_Result conditional = parseExpr(0);
        if (!conditional.has_value()) { return conditional; }

        consume();
        Node_Result body = parseBody();
        if (!body.has_value()) { return conditional; }

        Node else_node = Node::tombstone();
        if (check(TokenType::kw_else)) {
            // else if ...
            if (consume({TokenType::kw_if})) {
                Node_Result expected_else = parseIf();
                if (!expected_else.has_value()) { return std::unexpected(expected_else.error()); }
                else_node = expected_else.value();

                // else ...
            } else {
                Node_Result expected_else = parseBody();
                if (!expected_else.has_value()) { return std::unexpected(expected_else.error()); }
                else_node = expected_else.value();
            }
        }

        std::vector<Node> children = {conditional.value(), body.value()};
        if (else_node != Node::tombstone()) { children.push_back(else_node); }
        return Node(NodeType::ifNode, ifNode(children.size()), children);
    }

    [[nodiscard]] Node_Result Parser::parseLet() noexcept {
        if (!check(TokenType::kw_let)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_let"));
        }

        if (!consume({TokenType::ident})) {
            return std::unexpected(Error(ErrType::Parser, "No name found for let"));
        }

        const std::string name = current.toString(&L);

        if (!consume({TokenType::op_equal, TokenType::colon})) {
            return std::unexpected(
                Error(ErrType::Parser, "Malformed `let`: expected op_equal or colon"));
        }

        if (check(TokenType::colon)) {
            consume();

            if (!check(TokenType::ident)) {
                return std::unexpected(
                    Error(ErrType::Parser, "Malformed `let`: no type specified after colon"));
            }

            std::string type_lit = current.toString(&L);

            if (!consume({TokenType::op_equal, TokenType::semicolon})) {
                return std::unexpected(
                    Error(ErrType::Parser, "Malformed `let`: expected op_equal or semicolon"));
            }

            if (check(TokenType::semicolon)) {
                return Node(NodeType::varNode, varNode(0, name, type_lit));
            }

            consume();  // consume `=`
            Node_Result rhs = parseExpr(0);
            if (!rhs.has_value()) { return std::unexpected(rhs.error()); }
            return Node(NodeType::varNode, varNode(1, name, type_lit), {rhs.value()});
        }

        if (!consume({TokenType::kw_func})) {
            return std::unexpected(Error(ErrType::Parser, "Malformed `let`: No function found"));
        }

        bool isFunc = false;
        Node rhs = Node::tombstone();
        if (check(TokenType::kw_func)) {
            Node_Result func = parseFunc();
            if (!func.has_value()) { return std::unexpected(func.error()); }

            rhs = func.value();
            isFunc = true;
        } else {
            // TODO: let variables
        }

        return Node(NodeType::letNode, letNode(name, isFunc), {rhs});
    }

    [[nodiscard]] Node_Result Parser::parseNumLit() noexcept {
        if (!check(TokenType::num_literal)) {
            return std::unexpected(
                Error(ErrType::Parser, "Unexpected token: expected num literal"));
        }

        return Node(NodeType::numlitNode, numlitNode(current.toNum(&L)));
    }

    [[nodiscard]] Node_Result Parser::parseParam() noexcept {
        if (!check(TokenType::ident)) {
            return std::unexpected(
                Error(ErrType::Parser, "Unexpected token: expected parameter ident"));
        }

        std::string name = current.toString(&L);

        if (!consume({TokenType::colon})) {
            return std::unexpected(
                Error(ErrType::Parser, "Unexpected token: parameter type not set"));
        }
        std::string type = current.toString(&L);

        consume();
        return Node(NodeType::paramNode, paramNode(name, type));
    }

    [[nodiscard]] Node_Result Parser::parseReturn() noexcept {
        if (!check(TokenType::kw_return)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_return"));
        }

        consume();
        Node_Result expr = parseExpr(0);
        if (!expr.has_value()) { return std::unexpected(expr.error()); }

        if (check(TokenType::semicolon)) { consume(); }

        return Node(NodeType::returnNode, returnNode(), {expr.value()});
    }

    [[nodiscard]] Node_Result Parser::parseStrLit() noexcept {
        if (!check(TokenType::str_literal)) {
            return std::unexpected(
                Error(ErrType::Parser, "Unexpected token: expected str_literal"));
        }

        // try to strip off the quotes
        current.start++;
        current.len -= 2;

        return Node(NodeType::strLitNode, strLitNode(current.toString(&L)));
    }

    [[nodiscard]] Node_Result Parser::parseVariable() noexcept {
        return std::unexpected(Error(ErrType::NotImplemented, "parseVariable"));
    }

    [[nodiscard]] std::expected<std::vector<Node>, Error> Parser::operator()() {
        std::vector<Node> code = {};

        consume();  // start
        while (!check(TokenType::eof)) {
            if (current.type == TokenType::kw_let) {
                Node_Result expected = parseLet();
                if (!expected.has_value()) { return std::unexpected(expected.error()); }
                code.push_back(expected.value());
            } else {
                return std::unexpected(
                    Error(ErrType::Parser, "Unexpected token found. Expected top-level keyword"));
            }
        }

        return code;
    }

    void Parser::display_syntax_tree(const std::vector<Node>& tree) const noexcept {
        auto helper = [](this auto self, const Node x, const int offset) -> void {
            std::print("{}", std::string(offset, ' '));

            std::visit([](auto&& v) { std::println("{}", v.display()); }, x.data);
            for (auto child : x.children) { self(child, offset + 2); }
        };

        std::println("=== PARSER ===");
        for (Node elem : tree) { helper(elem, 0); }
        std::println();
    }

}  // namespace Winter
