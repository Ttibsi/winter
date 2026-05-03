#include "parser.h"

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

    [[nodiscard]] Node_Result Parser::parseParam() noexcept {
        if (!check(TokenType::ident)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected ident"));
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

    [[nodiscard]] Node_Result Parser::parseBody() noexcept {
        if (!consume({TokenType::lbrace})) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected ident"));
        }

        std::vector<Node> children = {};
        while (!check(TokenType::rbrace)) {
            if (check(TokenType::kw_return)) {
                Node_Result maybe_return = parseReturn();
                if (!maybe_return.has_value()) { return std::unexpected(maybe_return.error()); }
                children.push_back(maybe_return.value());
            }

            return std::unexpected(Error(ErrType::Parser, "Token not known in body"));
        }

        return Node(NodeType::bodyNode, bodyNode(children.size()), children);
    }

    [[nodiscard]] Node_Result Parser::parseFunc() noexcept {
        if (!check(TokenType::kw_func)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected func"));
        }

        if (!consume({TokenType::lparen})) {
            return std::unexpected(
                Error(ErrType::Parser, "Unexpected token: func arguments not specified"));
        }

        // Contents
        std::vector<Node> parameters = {};
        std::string retType;

        while (!check(TokenType::rparen)) {
            auto param = parseParam();
            if (!param.has_value()) { return std::unexpected(param.error()); }

            parameters.push_back(param.value());
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

        if (!consume({TokenType::rbrace})) {
            return std::unexpected(Error(ErrType::Parser, "function body never closed"));
        }

        return Node(NodeType::funcNode, funcNode(parameters, retType), {expected_body.value()});
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
            // TODO: handle type literals
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

    [[nodiscard]] Node_Result Parser::parseReturn() noexcept {
        if (!check(TokenType::kw_return)) {
            return std::unexpected(Error(ErrType::Parser, "Unexpected token: expected kw_return"));
        }

        consume();
        Node_Result expr = parseExpr();
        if (!expr.has_vale()) { return std::unexpected(expr.error()); }

        return Node();
    }

    [[nodiscard]] std::expected<std::vector<Node>, Error> Parser::operator()() {
        std::vector<Node> code = {};

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

    // void Parser::display_syntax_tree(const std::vector<Node>& tree) const noexcept {
    //     auto helper = [](const auto x) {};
    //     for (auto elem : tree) { helper(elem); }
    // }

}  // namespace Winter
