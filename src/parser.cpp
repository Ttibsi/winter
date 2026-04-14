#include "parser.h"

#include <expected>
#include <variant>

#include "error.h"

namespace Winter {
    [[nodiscard]] auto Parser::parse() -> std::expected<AST, Error> {
        while (next()) {
            token_result_t ret = L.tokens.back();
            if (!ret.has_value()) { return std::unexpected(ret.error()); }

            if (ret.value().type == TokenType::MOD) {
                const std::optional<Error> e = parseMod();
                if (e.has_value()) { return std::unexpected(e.value()); }
            } else if (ret.value().type == TokenType::ALIAS) {
                const std::optional<Error> e = parseAlias();
                if (e.has_value()) { return std::unexpected(e.value()); }
            } else if (ret.value().type == TokenType::TYPE) {
                const std::optional<Error> e = parseTypeDefinition();
                if (e.has_value()) { return std::unexpected(e.value()); }
            } else if (ret.value().type == TokenType::LET) {
                const std::optional<Error> e = parseLet();
                if (e.has_value()) { return std::unexpected(e.value()); }
            } else {
                return std::unexpected(
                    Error(ErrType::Parser, "Unexpected token found at top level"));
            }
        }

        return ast;
    }

    [[nodiscard]] auto Parser::next() -> std::optional<Error> {
        auto ret = L(L.src);
        if (ret.has_value()) {
            current_token = ret.value();
            return std::nullopt;
        }

        return ret.error();
    }

    [[nodiscard]] auto Parser::parseMod() -> std::optional<Error> {
        if (current_token.type != TokenType::MOD) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: MOD");
        }

        auto ret = next();  // Advance to string;
        if (ret.has_value()) { return ret.value(); }

        std::size_t id = ast.makeMod(std::string(current_token.toString(L.src)));
        ret = next();
        if (ret.has_value()) { return ret.value(); }

        if (current_token.type != TokenType::SEMICOLON) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: SEMICOLON");
        }

        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseAlias() -> std::optional<Error> {
        if (current_token.type != TokenType::ALIAS) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: ALIAS");
        }

        auto ret = next();  // Advance to name;
        if (ret.has_value()) { return ret.value(); }
        const std::string_view name = current_token.toString(L.src);

        ret = next();  // Advance to eq;
        if (ret.has_value()) { return ret.value(); }
        if (current_token.type != TokenType::EQUAL) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: EQUAL");
        }

        std::string type_real = std::string(current_token.toString(L.src));
        ret = next();  // Advance to eq;
        if (ret.has_value()) { return ret.value(); }
        while (current_token.type != TokenType::SEMICOLON) {
            type_real += current_token.toString(L.src);
            type_real += " ";
            ret = next();  // Advance to eq;
            if (ret.has_value()) { return ret.value(); }
        }

        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseGenerics() -> std::expected<std::vector<std::string>, Error> {
        if (current_token.type != TokenType::LBRACE) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LBRACE"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        std::vector<std::string> generics = {};
        while (current_token.type != TokenType::RBRACE) {
            generics.push_back(std::string(current_token.toString(L.src)));
            auto ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }

            if (current_token.type == TokenType::COMMA) {
                ret = next();
                if (ret.has_value()) { return std::unexpected(ret.value()); }
            }
        }

        return generics;
    }

    [[nodiscard]] auto Parser::parseBody() -> std::optional<Error> {
        if (current_token.type != TokenType::LBRACE) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: LBRACE");
        }

        std::size_t depth = 1;
        while (depth > 0) {
            auto ret = next();
            if (ret.has_value()) { return ret.value(); }

            if (current_token.type == TokenType::LBRACE) {
                depth++;
            } else if (current_token.type == TokenType::RBRACE) {
                depth--;
            }
        }

        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseFunction() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::FUNC) {
            return std::unexpected(Error(ErrType::Parser, "Incorrect token found. Expected: FUNC"));
        }

        FuncDef func = FuncDef();
        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        // Parse optional function generics.
        if (current_token.type == TokenType::LSQUACKET) {
            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }

            while (current_token.type != TokenType::RSQUACKET) {
                func.generics.push_back(std::string(current_token.toString(L.src)));

                ret = next();
                if (ret.has_value()) { return std::unexpected(ret.value()); }
                if (current_token.type == TokenType::COMMA) {
                    ret = next();
                    if (ret.has_value()) { return std::unexpected(ret.value()); }
                }
            }

            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
        }

        // Parse function parameters.
        if (current_token.type != TokenType::LPAREN) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LPAREN"));
        }

        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        while (current_token.type != TokenType::RPAREN) {
            if (current_token.type != TokenType::IDENT) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: IDENT"));
            }

            const std::string name = std::string(current_token.toString(L.src));
            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }

            if (current_token.type != TokenType::COLON) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: COLON"));
            }

            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            if (current_token.type != TokenType::TYPE_LITERAL &&
                current_token.type != TokenType::IDENT) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: TYPE_LITERAL"));
            }

            func.args.emplace_back(name, std::string(current_token.toString(L.src)));

            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            if (current_token.type == TokenType::COMMA) {
                ret = next();
                if (ret.has_value()) { return std::unexpected(ret.value()); }
            }
        }

        // Parse the return type.
        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        if (current_token.type != TokenType::TYPE_LITERAL &&
            current_token.type != TokenType::IDENT) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: TYPE_LITERAL"));
        }
        func.return_type = std::string(current_token.toString(L.src));

        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        if (current_token.type != TokenType::LBRACE) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LBRACE"));
        }

        auto body_ret = parseBody();
        if (body_ret.has_value()) { return std::unexpected(body_ret.value()); }

        return ast.makeFunc(func);
    }

    [[nodiscard]] auto Parser::parseAttributeMethod() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::LET) {
            return std::unexpected(Error(ErrType::Parser, "Incorrect token found. Expected: LET"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        std::string name = std::string(current_token.toString(L.src));
        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        if (current_token.type == TokenType::EQUAL) {
            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }

            if (current_token.type == TokenType::FUNC) { return parseFunction(); }

            return std::unexpected(
                Error(ErrType::Parser, "Only function methods are currently implemented"));
        } else if (current_token.type == TokenType::COLON) {
            return std::unexpected(Error(ErrType::Parser, "Attribute parsing is not implemented"));
        }

        return std::unexpected(Error(ErrType::Parser, "parseAttributeMethod is not implemented"));
    }

    [[nodiscard]] auto Parser::parseClass() -> std::optional<Error> {
        if (current_token.type != TokenType::CLASS) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: CLASS");
        }

        ClassDef cls = ClassDef();
        auto generic_ret = parseGenerics();
        if (generic_ret.has_value()) {
            cls.generics = generic_ret.value();
        } else {
            return generic_ret.error();
        }

        auto ret = next();
        if (ret.has_value()) { return ret.value(); }
        if (current_token.type == TokenType::IMPLEMENTS) {
            auto ret = next();
            if (ret.has_value()) { return ret.value(); }
            cls.interface = current_token.toString(L.src);
            ret = next();
            if (ret.has_value()) { return ret.value(); }
        }

        if (current_token.type != TokenType::LBRACE) {
            return Error(ErrType::Parser, "No body found for class");
        }

        while (current_token.type != TokenType::RBRACE) {
            auto contents = parseAttributeMethod();
            if (!contents.has_value()) { return contents.error(); }
            cls.members.push_back(contents.value());

            auto ret = next();
            if (ret.has_value()) { return ret.value(); }
        }

        ast.makeClass(cls);
        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseEnum() -> std::optional<Error> {
        if (current_token.type != TokenType::ENUM) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: ENUM");
        }

        auto ret = next();
        if (ret.has_value()) { return ret.value(); }
        if (current_token.type != TokenType::LBRACE) {
            return Error(ErrType::Parser, "No body found for class");
        }

        ret = next();
        if (ret.has_value()) { return ret.value(); }
        std::vector<std::string> enumerations = {};
        while (current_token.type != TokenType::RBRACE) {
            enumerations.push_back(std::string(current_token.toString(L.src)));
            ret = next();
            if (ret.has_value()) { return ret.value(); }

            if (current_token.type == TokenType::COMMA) { next(); }
        }

        ast.makeEnum(enumerations);
        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseInterface() -> std::optional<Error> {}

    [[nodiscard]] auto Parser::parseTypeDefinition() -> std::optional<Error> {
        if (current_token.type != TokenType::TYPE) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: TYPE");
        }

        auto ret = next();  // advance to name;
        if (ret.has_value()) { return ret.value(); }

        const std::string_view name = current_token.toString(L.src);

        ret = next();  // Advance to eq;
        if (ret.has_value()) { return ret.value(); }

        if (current_token.type != TokenType::EQUAL) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: EQUAL");
        }

        const std::string_view keyword = current_token.toString(L.src);

        if (keyword == "class"sv) {
            std::optional<Error> e = parseClass();
            if (e.has_value()) { return e.value(); }
        } else if (keyword == "enum"sv) {
            std::optional<Error> e = parseEnum();
            if (e.has_value()) { return e.value(); }
        } else if (keyword == "interface"sv) {
            std::optional<Error> e = parseInterface();
            if (e.has_value()) { return e.value(); }
        } else {
            return Error(ErrType::Parser, "Incorrect keyword found in type");
        }

        if (current_token.type != TokenType::SEMICOLON) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: SEMICOLON");
        }

        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseLet() -> std::optional<Error> {
        if (current_token.type != TokenType::LET) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: LET");
        }

        auto ret = next();  // advance to name;
        if (ret.has_value()) { return ret.value(); }
        const std::string_view name = current_token.toString(L.src);

        ret = next();  // Advance to eq;
        if (ret.has_value()) { return ret.value(); }
        if (current_token.type != TokenType::EQUAL) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: EQUAL");
        }

        ret = next();
        if (ret.has_value()) { return ret.value(); }

        if (current_token.type == TokenType::FUNC) {
            auto func_ret = parseFunction();
            if (!func_ret.has_value()) { return func_ret.error(); }
            return std::nullopt;
        }

        return Error(ErrType::Parser, "Only function let bindings are currently implemented");
    }

}  // namespace Winter
