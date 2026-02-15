#ifndef WINTER_PARSER_H
#define WINTER_PARSER_H

#include "ast.h"
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

        [[nodiscard]] auto parseAlias() -> aliasNode;
        [[nodiscard]] auto parseMod(moduleNode*) -> void;
        [[nodiscard]] auto parseType() -> typeNode;
        [[nodiscard]] auto parseLet() -> letNode;
        [[nodiscard]] auto parse() -> moduleNode;
    };
}  // namespace Winter

#endif  // WINTER_PARSER_H
