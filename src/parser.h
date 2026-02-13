#ifndef WINTER_PARSER_H
#define WINTER_PARSER_H

#include <ast.h>

#include "lexer.h"

namespace Winter {
    struct Parser {
        Lexer* L;
        Token curr;
        Token next;

        explicit Parser(Lexer* lexer) : L(lexer), curr(lexer()), next(lexer()) {}

        [[nodiscard]] auto parse() -> moduleNode;
    };
}  // namespace Winter

#endif  // WINTER_PARSER_H
