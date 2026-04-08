#include <expected>
#include <optional>

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
        [[nodiscard]] auto parseMod() -> std::optional<Error>;
        [[nodiscard]] auto parseAlias() -> std::optional<Error>;
        [[nodiscard]] auto parseTypeDefinition() -> std::optional<Error>;
        [[nodiscard]] auto parseLet() -> std::optional<Error>;
    };
}  // namespace Winter
