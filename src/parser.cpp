#include "parser.h"

#include <expected>
#include <variant>

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

        L();  // Advance to string

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

        L();  // Advance to name
        const std::string_view name = current_token.toString(L.src);

        L();  // Advance to eq
        if (current_token.type != TokenType::EQUAL) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: EQUAL");
        }

        std::string type_real = current_token.toString(L.src);
        L();  // Advance to eq
        while (current_token.type != TokenType::SEMICOLON) {
            type_real += current_token.toString(L.src);
            type_real += " ";
            L();  // Advance to eq
        }

        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseGenerics() -> std::optional<Error> {}
    [[nodiscard]] auto Parser::parseAttributeMethod() -> std::expected<AttrMethod, Error> {}

    [[nodiscard]] auto Parser::parseClass() -> std::optional<Error> {
        if (current_token.type != TokenType::CLASS) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: CLASS");
        }

        ClassDef cls = ClassDef();
        cls.generics = parseGenerics();

        L();
        if (current_token.type == TokenType::IMPLEMENTS) {
            L();

            // TODO: find the interface, extend the attributes and methods

            L();
        }

        if (current_token.type != TokenType::LBRACE) {
            return Error(ErrType::Parser, "No body found for class");
        }

        while (current_token != TokenType::RBRACE) {
            auto contents = parseAttributeMethod();
            if (!contents.has_value()) { return std::unexpected(contents.error()); }

            if (std::holds_alternative<AttrDef>(contents.value())) {
                cls.attributes.push_back(std::get<AttrDef>(contents.value()));
            } else if (std::holds_alternative<MethodDef>(contents.value())) {
                cls.methods.push_back(std::get<MethodDef>(contents.value()));
            }

            L();
        }

        ast.makeClass(cls);
        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseEnum() -> std::optional<Error> {}
    [[nodiscard]] auto Parser::parseInterface() -> std::optional<Error> {}

    [[nodiscard]] auto Parser::parseTypeDefinition() -> std::optional<Error> {
        if (current_token.type != TokenType::TYPE) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: TYPE");
        }

        L();  // advance to name
        const std::string_view name = current_token.toString(L.src);

        L();  // Advance to eq
        if (current_token.type != TokenType::EQUAL) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: EQUAL");
        }

        const std::string_view keyword = current_token.toString(L.src);
        const TypeInner inner;
        switch (keyword) {
            case "class":     inner = parseClass(); break;
            case "enum":      inner = parseEnum(); break;
            case "interface": inner = parseInterface(); break;
        };

        if (current_token.type != TokenType::SEMICOLON) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: SEMICOLON");
        }

        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseLet() -> std::optional<Error> {
        if (current_token.type != TokenType::LET) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: LET");
        }

        L();  // advance to name
        const std::string_view name = current_token.toString(L.src);

        L();  // Advance to eq
        if (current_token.type != TokenType::EQUAL) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: EQUAL");
        }
    }

}  // namespace Winter
