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

    [[nodiscard]] auto Parser::parseMod() -> std::optional<Error> {
        if (current_token.type != TokenType::MOD) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: MOD");
        }

        L(); // Advance to string

        std::size_t id = ast.makeMod(std::string(current_token.toString(L.src)));
        L();

        if (current_token.type != TokenType::SEMICOLON) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: SEMICOLON");
        }

        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseAlias() -> std::optional<Error> {
        if (current_token.type != TokenType::ALIAS) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: ALIAS");
        }

        L(); // Advance to name 
        const std::string_view name = current_token.toString(L.src);

        L(); // Advance to eq 
        if (current_token.type != TokenType::EQUAL) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: EQUAL");
        }

        std::string type_real = current_token.toString(L.src);
        L(); // Advance to eq 
        while (current_token.type != TokenType::SEMICOLON) {
            type_real += current_token.toString(L.src);
            type_real += " ";
            L(); // Advance to eq 
        }
        
        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseTypeDefinition() -> std::optional<Error> {}
    [[nodiscard]] auto Parser::parseLet() -> std::optional<Error> {}
}  // namespace Winter
