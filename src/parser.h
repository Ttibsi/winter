#include <expected>
#include <optional>
#include <string>
#include <vector>

#include "ast.h"
#include "error.h"
#include "lexer.h"

namespace Winter {
    struct Parser {
        Lexer L;
        AST ast;
        Token current_token;

        [[nodiscard]] constexpr explicit Parser()
            : L(Lexer()), ast(AST()), current_token(Token()) {}

        [[nodiscard]] auto parse() -> std::expected<AST, Error>;
        [[nodiscard]] auto next() -> std::optional<Error>;
        [[nodiscard]] auto parseMod() -> std::optional<Error>;
        [[nodiscard]] auto parseAlias() -> std::optional<Error>;
        [[nodiscard]] auto parseGenerics() -> std::expected<std::vector<std::string>, Error>;
        [[nodiscard]] auto parseBody() -> std::expected<std::vector<std::size_t>, Error>;
        [[nodiscard]] auto parseIf() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseFor() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseForEach(std::string loop_var) -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseSwitch() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseCaseArm() -> std::expected<CaseArm, Error>;
        [[nodiscard]] auto parseDefaultArm() -> std::expected<std::vector<std::size_t>, Error>;
        [[nodiscard]] auto parseReturn() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseBreak() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseContinue() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseExpression() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseConst() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseFunction() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseAttributeMethod() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseClass() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseEnum() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseInterface() -> std::expected<std::size_t, Error>;
        [[nodiscard]] auto parseInterfaceAttribute(InterfaceDef&, const std::string&)
            -> std::optional<Error>;
        [[nodiscard]] auto parseInterfaceMethod(InterfaceDef&, const std::string&)
            -> std::optional<Error>;
        [[nodiscard]] auto parseTypeDefinition() -> std::optional<Error>;
        [[nodiscard]] auto parseLet() -> std::expected<std::size_t, Error>;
    };
}  // namespace Winter
