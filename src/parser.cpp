#include "parser.h"

#include <expected>

#include "error.h"

namespace Winter {
    [[nodiscard]] auto Parser::parse() -> std::expected<AST, Error> {
        while (L()) {
            token_result_t ret = L.tokens.back();
            if (!ret.has_value()) { return std::unexpected(ret.error()); }

            if (ret.value().type == TokenType::MOD) {
                const Error e = parseMod();
                if (!e.isNone()) { return std::unexpected(e); }
            } else if (ret.value().type == TokenType::ALIAS) {
                const Error e = parseAlias();
                if (!e.isNone()) { return std::unexpected(e); }
            } else if (ret.value().type == TokenType::TYPE) {
                const Error e = parseTypeDefinition();
                if (!e.isNone()) { return std::unexpected(e); }
            } else if (ret.value().type == TokenType::LET) {
                const Error e = parseLet();
                if (!e.isNone()) { return std::unexpected(e); }
            } else {
                return std::unexpected(
                    Error(ErrType::Parser, "Unexpected token found at top level"));
            }
        }

        return ast;
    }

    [[nodiscard]] auto Parser::parseMod() -> Error {
        if (current_token.type != TokenType::MOD) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: MOD");
        }

        L();

        ast.
    }

    [[nodiscard]] auto Parser::parseAlias() -> Error {}
    [[nodiscard]] auto Parser::parseTypeDefinition() -> Error {}
    [[nodiscard]] auto Parser::parseLet() -> Error {}
}  // namespace Winter
