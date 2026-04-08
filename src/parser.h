#include <expected>

#include "ast.h"
#include "error.h"
#include "lexer.h"

namespace Winter {
    struct Parser {
        Lexer L;
        AST ast;
        Token current_token;

        [[nodiscard]] constexpr explicit Parser() {
            L = Lexer();
            ast = AST();
        }

        [[nodiscard]] auto parse() -> std::expected<AST, Error>;
        [[nodiscard]] auto parseMod() -> Error;
        [[nodiscard]] auto parseAlias() -> Error;
        [[nodiscard]] auto parseTypeDefinition() -> Error;
        [[nodiscard]] auto parseLet() -> Error;
    };
}  // namespace Winter
