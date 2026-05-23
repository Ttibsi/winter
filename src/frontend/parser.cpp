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

    [[nodiscard]] Node_Result Parser::parseAlias() noexcept {
        if (!check(TokenType::kw_alias)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_alias"));
        }
        consume();

        const std::string ident = current.toString(&L);

        if (!consume({TokenType::op_equal})) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: alias not set"));
        }
        consume();  // consume op_equal

        if (check(TokenType::kw_func)) {
            // function alias
            if (!consume({TokenType::lparen})) {
                return std::unexpected(
                    Error(ErrType::Parser, "Unexpected token: no params found in function alias"));
            }
            consume();  // consume lparen

            std::vector<Node> params = {};
            while (!check(TokenType::rparen)) {
                params.push_back(Node(NodeType::identNode, identNode(current.toString(&L))));
                consume();
                if (check(TokenType::comma)) { consume(); }
            }

            consume();  // consume rparen
            const int paramcount = static_cast<int>(params.size());
            if (!check(TokenType::semicolon)) {
                params.push_back(Node(NodeType::identNode, identNode(current.toString(&L))));
                consume();
            }

            consume();  // consume semicolon
            Node f = Node(NodeType::funcAlias, funcAlias(paramcount), params);
            return Node(NodeType::aliasNode, aliasNode(ident, aliasNode::childType::func), {f});

        } else {
            // type alias
            Node t = Node(NodeType::typeAlias, typeAlias(current.toString(&L)));
            consume();  // consume ident
            consume();  // consume semicolon;
            return Node(NodeType::aliasNode, aliasNode(ident, aliasNode::childType::type), {t});
        }

        return std::unexpected(Error(ErrType::Parser, "Unexpected error in parsing alias"));
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
            Node_Result maybe_return =
                std::unexpected(Error(ErrType::Parser, "Token not known in body"));

            if (check(TokenType::kw_return)) {
                maybe_return = parseReturn();

            } else if (check(TokenType::ident)) {
                maybe_return = parseCallOrVariable();

            } else if (check(TokenType::kw_if)) {
                maybe_return = parseIf();

            } else if (check(TokenType::kw_let)) {
                Node_Result maybe_return = parseLet(false);
                if (!maybe_return.has_value()) { return std::unexpected(maybe_return.error()); }
                children.push_back(maybe_return.value());
                consume();  // consume ';'
                continue;

            } else if (check(TokenType::kw_for)) {
                maybe_return = parseFor();

            } else if (check(TokenType::kw_const)) {
                maybe_return = parseConst();

            } else if (check(TokenType::kw_switch)) {
                maybe_return = parseSwitch();

            } else if (check(TokenType::kw_type)) {
                maybe_return = parseType();
                consume();  // consume final rbrace
            }

            if (!maybe_return.has_value()) { return std::unexpected(maybe_return.error()); }
            children.push_back(maybe_return.value());
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

    [[nodiscard]] Node_Result Parser::parseCase() noexcept {
        if (!(check(TokenType::kw_case) || check(TokenType::kw_default))) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_case"));
        }
        const bool default_case = (current.type == TokenType::kw_default);
        consume();  // consume kw_case

        std::string ident = "";
        if (!default_case) {
            ident = current.toString(&L);
            consume();
        }

        if (!default_case && check(TokenType::kw_fallthrough)) {
            consume();  // consume kw_fallthrough
            consume();  // consume semicolon

            Node_Result case_node = parseCase();
            if (!case_node.has_value()) { return std::unexpected(case_node.error()); }
            return Node(NodeType::caseNode, caseNode(true, ident, false), {case_node.value()});
        } else if (check(TokenType::lbrace)) {
            Node_Result body_node = parseBody();
            if (!body_node.has_value()) { return std::unexpected(body_node.error()); }
            return Node(
                NodeType::caseNode, caseNode(false, ident, default_case), {body_node.value()});
        }

        return std::unexpected(Error(ErrType::Parser, "No body found for case statement"));
    }

    [[nodiscard]] Node_Result Parser::parseConst() noexcept {
        if (!check({TokenType::kw_const})) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_const"));
        }

        consume();

        if (check(TokenType::kw_let)) {
            Node_Result maybe_return = parseLet(true);
            if (!maybe_return.has_value()) { return std::unexpected(maybe_return.error()); }
            consume();  // consume ';'
            return maybe_return.value();
        } else {
            return std::unexpected(Error(ErrType::Parser, "Only let statements can be const"));
        }
    }

    [[nodiscard]] Node_Result Parser::parseEnum() noexcept {
        if (!check({TokenType::kw_enum})) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_enum"));
        }

        if (!consume({TokenType::lbrace})) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected lbrace"));
        }
        consume();

        std::vector<Node> idents = {};
        while (!check(TokenType::rbrace)) {
            std::string ident = current.toString(&L);
            idents.push_back(Node(NodeType::identNode, identNode(ident)));
            consume();

            if (check(TokenType::comma)) { consume(); }
            if (!check(TokenType::ident)) { break; }
        }

        return Node(NodeType::enumNode, enumNode(static_cast<int>(idents.size())), idents);
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

            case TokenType::ident: {
                std::string ident = current.toString(&L);
                lhs = Node(NodeType::identNode, identNode(ident));
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
            case TokenType::rparen:    break;
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

    [[nodiscard]] Node_Result Parser::parseFor() noexcept {
        if (!check(TokenType::kw_for)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_for"));
        }

        if (!consume({TokenType::lparen})) {
            return std::unexpected(Error(ErrType::Parser, "No lparen found in for-block"));
        }
        consume();  // consume lparen

        std::vector<Node> children = {};

        if (check(TokenType::kw_let)) {
            // basic for-loop
            Node_Result start = parseLet(false);
            if (!start.has_value()) { return std::unexpected(start.error()); }
            consume();  // consume ';'
            Node_Result stop = parseExpr(0);
            if (!stop.has_value()) { return std::unexpected(stop.error()); }
            consume();  // consume ';'
            Node_Result step = parseExpr(0);
            if (!step.has_value()) { return std::unexpected(step.error()); }

            children = {start.value(), stop.value(), step.value()};
        } else if (check(TokenType::ident)) {
            Node ident = Node(NodeType::identNode, identNode(current.toString(&L)));

            if (!consume({TokenType::colon})) {
                return std::unexpected(Error(ErrType::Parser, "No container found in for-each"));
            }
            consume();

            Node container = Node(NodeType::identNode, identNode(current.toString(&L)));
            consume();

            children = {ident, container};
        } else {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found when parsing kw_for"));
        }

        if (!consume({TokenType::lbrace})) {
            return std::unexpected(Error(ErrType::Parser, "No lbrace found after block"));
        }

        Node_Result body = parseBody();
        if (!body.has_value()) { return std::unexpected(body.error()); }
        children.push_back(body.value());

        return Node(NodeType::forNode, forNode(), children);
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

    [[nodiscard]] Node_Result Parser::parseLet(const bool isConst) noexcept {
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
                return Node(NodeType::varNode, varNode(0, name, type_lit, isConst));
            }

            consume();  // consume `=`
            Node_Result rhs = parseExpr(0);
            if (!rhs.has_value()) { return std::unexpected(rhs.error()); }
            return Node(NodeType::varNode, varNode(1, name, type_lit, isConst), {rhs.value()});
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

        return Node(NodeType::letNode, letNode(name, isFunc, isConst), {rhs});
    }

    [[nodiscard]] Node_Result Parser::parseMod() noexcept {
        if (!check(TokenType::kw_mod)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_mod"));
        }

        if (!consume({TokenType::ident})) {
            return std::unexpected(Error(ErrType::Parser, "No module name found"));
        }

        std::string name = current.toString(&L);

        if (!consume({TokenType::semicolon})) {
            return std::unexpected(
                Error(ErrType::Parser, "Module never completed. Please insert `;`"));
        }
        consume();  // consume semicolon

        return Node(NodeType::modNode, modNode(name));
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

    [[nodiscard]] Node_Result Parser::parseSwitch() noexcept {
        if (!check(TokenType::kw_switch)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_switch"));
        }

        if (!consume({TokenType::lparen})) {
            // TODO: Better error message here
            return std::unexpected(Error(ErrType::Parser, "No switch variable defined"));
        }
        consume();  // consume lparen

        const std::string value = current.toString(&L);
        consume();  // consume ident
        consume();  // consume rparen
        consume();  // consume lbrace

        std::vector<Node> cases = {};
        while (check(TokenType::kw_case)) {
            Node_Result case_node = parseCase();
            if (!case_node.has_value()) { return std::unexpected(case_node.error()); }
            cases.push_back(case_node.value());
        }

        bool hasDefault = false;
        if (check(TokenType::kw_default)) {
            hasDefault = true;
            Node_Result case_node = parseCase();
            if (!case_node.has_value()) { return std::unexpected(case_node.error()); }
            cases.push_back(case_node.value());
        }

        consume();  // consume '}'
        return Node(
            NodeType::switchNode, switchNode(value, static_cast<int>(cases.size()), hasDefault),
            cases);
    }

    [[nodiscard]] Node_Result Parser::parseType() noexcept {
        if (!check(TokenType::kw_type)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_type"));
        }
        consume();

        const std::string name = current.toString(&L);
        if (!consume({TokenType::op_equal})) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: No type body found"));
        }
        consume();

        Node_Result body = Node::tombstone();
        NodeType childType;

        switch (current.type) {
            case TokenType::kw_enum:
                body = parseEnum();
                childType = NodeType::enumNode;
                break;
            default: return std::unexpected(Error(ErrType::Parser, "Unexpected type found"));
        }

        if (!body.has_value()) { return std::unexpected(body.error()); }

        return Node(NodeType::typeNode, typeNode(childType), {body.value()});
    }

    [[nodiscard]] Node_Result Parser::parseVariable() noexcept {
        return std::unexpected(Error(ErrType::NotImplemented, "parseVariable"));
    }

    [[nodiscard]] std::expected<std::vector<Node>, Error> Parser::operator()() {
        std::vector<Node> code = {};

        consume();  // start
        while (!check(TokenType::eof)) {
            Node_Result expected = std::unexpected(
                Error(ErrType::Parser, "Unexpected token found. Expected top-level keyword"));

            if (current.type == TokenType::kw_let) {
                expected = parseLet(false);
            } else if (current.type == TokenType::kw_mod) {
                expected = parseMod();
            } else if (current.type == TokenType::kw_const) {
                expected = parseConst();
            } else if (current.type == TokenType::kw_alias) {
                expected = parseAlias();
            } else if (current.type == TokenType::kw_type) {
                expected = parseType();
                consume();
            }

            if (!expected.has_value()) { return std::unexpected(expected.error()); }
            code.push_back(expected.value());
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
