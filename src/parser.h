#ifndef WINTER_PARSER_H
#define WINTER_PARSER_H

#include "ast.h"
#include "error.h"
#include "lexer.h"

namespace Winter {
    struct Parser {
        Lexer* L;
        Token curr;
        Token next;

        explicit Parser(Lexer* lexer) : L(lexer), curr(Token::ERR()), next(Token::ERR()) {
            auto getToken = [&]() {
                auto expectedToken = (*L)();
                if (!expectedToken.has_value()) { return Token::ERR(); }
                return expectedToken.value();
            };

            curr = getToken();
            next = getToken();
        }

        [[nodiscard]] auto parseAlias() -> std::expected<aliasNode, Error>;
        [[nodiscard]] auto parseMod(moduleNode*) -> std::expected<void, Error>;
        [[nodiscard]] auto parseType() -> std::expected<typeNode, Error>;
        [[nodiscard]] auto parseLet() -> std::expected<letStmt, Error>;
        [[nodiscard]] auto match(const TokenType) -> bool;
        auto advance() -> void;
        [[nodiscard]] auto parse() -> std::expected<moduleNode, Error>;
    };
}  // namespace Winter

#endif  // WINTER_PARSER_H
