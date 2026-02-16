#include "parser.h"

#include <expected>
#include <format>

namespace Winter {
    [[nodiscard]] auto Parser::parseAlias() -> std::expected<aliasNode, Error> {
        if (!match(TokenType::ALIAS)) {
            return std::unexpected(Error(ErrType::Parser, "`alias` statement not found"));
        }
        aliasNode node = {};
        advance();

        auto tok = (*L)();
        if (!tok.has_value()) { return std::unexpected(tok.error()); }
        node.name = tok.value().getString();

        if (!(match(TokenType::EQUAL))) {
            return std::unexpected(Error(ErrType::Parser, "Malformed alias statement"));
        }

        tok = (*L)();
        if (!tok.has_value()) { return std::unexpected(tok.error()); }
        node.type = tok.value().getString();

        advance();
        if (!match(TokenType::SEMICOLON)) {
            return std::unexpected(
                Error(ErrType::Parser, "Alias statement does not end in a semicolon"));
        }

        return node;
    }

    [[nodiscard]] auto Parser::parseExpr(const std::size_t bp) -> std::expected<Expr_t, Error> {}
    [[nodiscard]] auto Parser::parseFunc() -> std::expected<funcNode, Error> {}
    [[nodiscard]] auto Parser::parseInclude() -> std::expected<includeNode, Error> {}

    [[nodiscard]] auto Parser::parseLet() -> std::expected<letStmt, Error> {
        if (!match(TokenType::LET)) {
            return std::unexpected(Error(ErrType::Parser, "`let` statement not found"));
        }
        advance();

        letStmt node = {};
        node.name = curr.getString();

        advance();
        if (!expect({TokenType::COLON, TokenType::EQUAL}) {
            return std::unexpected(Error(ErrType::Parser, "Malformed let statement"));
        }

        if (match(TokenType::COLON)) {
            advance();
            node.typeLiteral = curr.getString();
            advance();
        }

        if (!match(TokenType::EQUAL)) {
            return std::unexpected(
                Error(ErrType::Parser, "Malformed let statement: no equal sign"));
        }

        advance();
        switch (curr.type) {
            case TokenType::INCLUDE: {
                auto expected = parseInclude();
                if (!expected.has_value()) { return std::unexpected(expected.error()); }
                node.body = expected.value();
            } break;

            case TokenType::FUNC: {
                auto expected = parseFunc();
                if (!expected.has_value()) { return std::unexpected(expected.error()); }
                node.body = expected.value();
            } break;

            default: {
                auto expected = parseExpr();
                if (!expected.has_value()) { return std::unexpected(expected.error()); }
                node.body = expected.value();
            } break;
        };

        return node;
    }

    [[nodiscard]] auto Parser::parseMod(moduleNode* mod) -> std::expected<void, Error> {
        advance();
        mod->moduleName = curr.getString();
        advance();
        if (!match(TokenType::SEMICOLON)) {
            return std::unexpected(Error(ErrType::Parser, "Malformed mod statement"));
        }

        return {};
    }

    [[nodiscard]] auto Parser::parseType() -> std::expected<typeNode, Error> {}

    [[nodiscard]] auto Parser::expect(std::initializer_list<TokenType> toks) -> bool {
        for (auto&& t : toks) {
            if (match(t)) { return true; }
        }

        return false;
    }

    [[nodiscard]] auto Parser::match(const TokenType type) -> bool {
        return curr.type == type;
    }

    auto Parser::advance() -> void {
        curr = next;
        auto expectedToken = (*L)();
        if (!expectedToken.has_value()) {
            next = Token::ERR();
            return;
        }

        next = expectedToken.value();
    }

    [[nodiscard]] auto Parser::parse() -> std::expected<moduleNode, Error> {
        moduleNode mod = {};

        while (!(curr.type == TokenType::ENDOFFILE || curr.type == TokenType::ERROR)) {
            switch (curr.type) {
                case TokenType::ALIAS: {
                    auto expected = parseAlias();
                    if (!expected.has_value()) { return std::unexpected(expected.error()); }
                    mod.aliases.push_back(expected.value());
                } break;

                case TokenType::MOD: {
                    auto expected = parseMod(&mod);
                    if (!expected.has_value()) { return std::unexpected(expected.error()); }
                } break;

                case TokenType::TYPE: {
                    auto expected = parseType();
                    if (!expected.has_value()) { return std::unexpected(expected.error()); }
                    mod.contents.push_back(expected.value());
                } break;

                case TokenType::LET: {
                    auto expected = parseLet();
                    if (!expected.has_value()) { return std::unexpected(expected.error()); }
                    mod.contents.push_back(expected.value());
                } break;

                default: {
                    return std::unexpected(Error(
                        ErrType::Parser, std::format("Token {} not handled", toString(curr.type))));
                }
            };
        };

        return mod;
    }
}  // namespace Winter
