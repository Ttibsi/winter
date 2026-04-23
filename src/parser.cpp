#include "parser.h"

#include <expected>
#include <variant>

#include "error.h"

namespace Winter {
    namespace {

        [[nodiscard]] auto parseClause(Parser& p, TokenType end)
            -> std::expected<std::size_t, Error> {
            int depth = 0;
            for (;;) {
                if (p.current_token.type == end && depth == 0) { return 0; }
                const auto t = p.current_token.type;
                if (t == TokenType::LPAREN || t == TokenType::LSQUACKET) {
                    ++depth;
                } else if (t == TokenType::RPAREN || t == TokenType::RSQUACKET) {
                    if (depth > 0) {
                        --depth;
                    } else {
                        return std::unexpected(
                            Error(ErrType::Parser, "Unexpected closing bracket in for head"));
                    }
                }
                auto ret = p.next();
                if (ret.has_value()) { return std::unexpected(ret.value()); }
            }
        }

        [[nodiscard]] auto parseForInitTypedLet(Parser& p) -> std::expected<std::size_t, Error> {
            if (p.current_token.type != TokenType::LET) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: LET"));
            }
            auto ret = p.next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            if (p.current_token.type != TokenType::IDENT) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: IDENT"));
            }
            const std::string name = std::string(p.current_token.toString(p.L.src));
            ret = p.next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            if (p.current_token.type != TokenType::COLON) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: COLON"));
            }
            ret = p.next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            if (p.current_token.type != TokenType::TYPE_LITERAL &&
                p.current_token.type != TokenType::IDENT) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: TYPE_LITERAL"));
            }
            std::string type_str = std::string(p.current_token.toString(p.L.src));
            ret = p.next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            if (p.current_token.type != TokenType::EQUAL) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: EQUAL"));
            }
            ret = p.next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            auto expr = p.parseExpression();
            if (!expr.has_value()) { return std::unexpected(expr.error()); }
            if (p.current_token.type != TokenType::SEMICOLON) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: SEMICOLON"));
            }
            ret = p.next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            return p.ast.makeLet(std::move(name), expr.value(), std::move(type_str));
        }

    }  // namespace

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
                auto e = parseLet();
                if (!e.has_value()) { return std::unexpected(e.error()); }
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

        ast.makeMod(std::string(current_token.toString(L.src)));
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

        ast.makeAlias(std::string(name), type_real);
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

    [[nodiscard]] auto Parser::parseBody() -> std::expected<std::vector<std::size_t>, Error> {
        if (current_token.type != TokenType::LBRACE) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LBRACE"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        std::vector<std::size_t> body;
        while (current_token.type != TokenType::RBRACE) {
            std::expected<std::size_t, Error> statement_idx;
            switch (current_token.type) {
                case TokenType::LET:      statement_idx = parseLet(); break;
                case TokenType::CONST:    statement_idx = parseConst(); break;
                case TokenType::IF:       statement_idx = parseIf(); break;
                case TokenType::FOR:      statement_idx = parseFor(); break;
                case TokenType::SWITCH:   statement_idx = parseSwitch(); break;
                case TokenType::RETURN:   statement_idx = parseReturn(); break;
                case TokenType::BREAK:    statement_idx = parseBreak(); break;
                case TokenType::CONTINUE: statement_idx = parseContinue(); break;
                default:                  statement_idx = parseExpression(); break;
            }

            if (!statement_idx.has_value()) { return std::unexpected(statement_idx.error()); }
            body.push_back(statement_idx.value());
        }

        auto next_ret = next();
        if (next_ret.has_value()) { return std::unexpected(next_ret.value()); }

        return body;
    }

    [[nodiscard]] auto Parser::parseIf() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::IF) {
            return std::unexpected(Error(ErrType::Parser, "Incorrect token found. Expected: IF"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        if (current_token.type != TokenType::LPAREN) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LPAREN"));
        }

        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        auto cond = parseExpression();
        if (!cond.has_value()) { return std::unexpected(cond.error()); }

        if (current_token.type != TokenType::RPAREN) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: RPAREN"));
        }
        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        auto then_body = parseBody();
        if (!then_body.has_value()) { return std::unexpected(then_body.error()); }

        std::optional<std::vector<std::size_t>> else_body = std::nullopt;
        if (current_token.type == TokenType::ELSE) {
            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            if (current_token.type == TokenType::IF) {
                auto else_if = parseIf();
                if (!else_if.has_value()) { return std::unexpected(else_if.error()); }
                else_body = std::vector<std::size_t> {else_if.value()};
            } else {
                auto else_blk = parseBody();
                if (!else_blk.has_value()) { return std::unexpected(else_blk.error()); }
                else_body = else_blk.value();
            }
        }

        return ast.makeIf(IfDef {cond.value(), then_body.value(), std::move(else_body)});
    }

    [[nodiscard]] auto Parser::parseFor() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::FOR) {
            return std::unexpected(Error(ErrType::Parser, "Incorrect token found. Expected: FOR"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        if (current_token.type != TokenType::LPAREN) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LPAREN"));
        }
        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        if (current_token.type == TokenType::IDENT) {
            std::string foreach_name = std::string(current_token.toString(L.src));
            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            if (current_token.type != TokenType::COLON) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: COLON"));
            }
            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            return parseForEach(std::move(foreach_name));
        }

        std::optional<std::size_t> init = std::nullopt;
        if (current_token.type == TokenType::SEMICOLON) {
            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
        } else if (current_token.type == TokenType::LET) {
            auto init_let = parseForInitTypedLet(*this);
            if (!init_let.has_value()) { return std::unexpected(init_let.error()); }
            init = init_let.value();
        } else {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found in for-loop init"));
        }

        std::optional<std::size_t> cond = std::nullopt;
        if (current_token.type != TokenType::SEMICOLON) {
            auto cond_clause = parseClause(*this, TokenType::SEMICOLON);
            if (!cond_clause.has_value()) { return std::unexpected(cond_clause.error()); }
            cond = cond_clause.value();
        }
        if (current_token.type != TokenType::SEMICOLON) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: SEMICOLON"));
        }
        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        std::optional<std::size_t> iter = std::nullopt;
        if (current_token.type != TokenType::RPAREN) {
            auto iter_clause = parseClause(*this, TokenType::RPAREN);
            if (!iter_clause.has_value()) { return std::unexpected(iter_clause.error()); }
            iter = iter_clause.value();
        }
        if (current_token.type != TokenType::RPAREN) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: RPAREN"));
        }
        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        auto body_cs = parseBody();
        if (!body_cs.has_value()) { return std::unexpected(body_cs.error()); }

        return ast.makeFor(CStyleFor {init, cond, iter, body_cs.value()});
    }

    [[nodiscard]] auto Parser::parseForEach(std::string loop_var)
        -> std::expected<std::size_t, Error> {
        auto it_clause = parseClause(*this, TokenType::RPAREN);
        if (!it_clause.has_value()) { return std::unexpected(it_clause.error()); }

        if (current_token.type != TokenType::RPAREN) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: RPAREN"));
        }
        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        auto body_fe = parseBody();
        if (!body_fe.has_value()) { return std::unexpected(body_fe.error()); }

        return ast.makeFor(ForeachFor {std::move(loop_var), it_clause.value(), body_fe.value()});
    }

    [[nodiscard]] auto Parser::parseSwitch() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::SWITCH) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: SWITCH"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        if (current_token.type != TokenType::LPAREN) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LPAREN"));
        }

        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        auto disc = parseExpression();
        if (!disc.has_value()) { return std::unexpected(disc.error()); }

        if (current_token.type != TokenType::RPAREN) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: RPAREN"));
        }
        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        if (current_token.type != TokenType::LBRACE) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LBRACE"));
        }
        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        SwitchDef sw = SwitchDef {disc.value(), {}, std::nullopt};
        while (current_token.type != TokenType::RBRACE) {
            if (current_token.type == TokenType::CASE) {
                auto arm = parseCaseArm();
                if (!arm.has_value()) { return std::unexpected(arm.error()); }
                sw.cases.push_back(arm.value());
            } else if (current_token.type == TokenType::DEFAULT) {
                auto def_body = parseDefaultArm();
                if (!def_body.has_value()) { return std::unexpected(def_body.error()); }
                if (sw.default_body.has_value()) {
                    return std::unexpected(
                        Error(ErrType::Parser, "Duplicate default arm in switch"));
                }
                sw.default_body = def_body.value();
            } else {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: CASE or DEFAULT"));
            }
        }

        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        return ast.makeSwitch(std::move(sw));
    }

    [[nodiscard]] auto Parser::parseCaseArm() -> std::expected<CaseArm, Error> {
        if (current_token.type != TokenType::CASE) {
            return std::unexpected(Error(ErrType::Parser, "Incorrect token found. Expected: CASE"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        auto label = parseExpression();
        if (!label.has_value()) { return std::unexpected(label.error()); }

        if (current_token.type != TokenType::LBRACE) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LBRACE"));
        }

        auto body = parseBody();
        if (!body.has_value()) { return std::unexpected(body.error()); }

        return CaseArm {label.value(), body.value()};
    }

    [[nodiscard]] auto Parser::parseDefaultArm() -> std::expected<std::vector<std::size_t>, Error> {
        if (current_token.type != TokenType::DEFAULT) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: DEFAULT"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        if (current_token.type != TokenType::LBRACE) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: LBRACE"));
        }

        return parseBody();
    }

    [[nodiscard]] auto Parser::parseReturn() -> std::expected<std::size_t, Error> {
        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        return 0;
    }

    [[nodiscard]] auto Parser::parseBreak() -> std::expected<std::size_t, Error> {
        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        return 0;
    }

    [[nodiscard]] auto Parser::parseContinue() -> std::expected<std::size_t, Error> {
        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        return 0;
    }

    [[nodiscard]] auto Parser::parseExpression() -> std::expected<std::size_t, Error> {
        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        return 0;
    }

    [[nodiscard]] auto Parser::parseConst() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::CONST) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: CONST"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        return parseLet();
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
        if (!body_ret.has_value()) { return std::unexpected(body_ret.error()); }
        func.body = body_ret.value();

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

    [[nodiscard]] auto Parser::parseClass() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::CLASS) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: CLASS"));
        }

        ClassDef cls = ClassDef();
        auto generic_ret = parseGenerics();
        if (generic_ret.has_value()) {
            cls.generics = generic_ret.value();
        } else {
            return std::unexpected(generic_ret.error());
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        if (current_token.type == TokenType::IMPLEMENTS) {
            auto ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            cls.interface = current_token.toString(L.src);
            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
        }

        if (current_token.type != TokenType::LBRACE) {
            return std::unexpected(Error(ErrType::Parser, "No body found for class"));
        }

        while (current_token.type != TokenType::RBRACE) {
            auto contents = parseAttributeMethod();
            if (!contents.has_value()) { return std::unexpected(contents.error()); }
            cls.members.push_back(contents.value());

            auto ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
        }

        return ast.makeClass(cls);
    }

    [[nodiscard]] auto Parser::parseEnum() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::ENUM) {
            return std::unexpected(Error(ErrType::Parser, "Incorrect token found. Expected: ENUM"));
        }

        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        if (current_token.type != TokenType::LBRACE) {
            return std::unexpected(Error(ErrType::Parser, "No body found for class"));
        }

        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        std::vector<std::string> enumerations = {};
        while (current_token.type != TokenType::RBRACE) {
            enumerations.push_back(std::string(current_token.toString(L.src)));
            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }

            if (current_token.type == TokenType::COMMA) {
                ret = next();
                if (ret.has_value()) { return std::unexpected(ret.value()); }
            }
        }

        return ast.makeEnum(enumerations);
    }

    [[nodiscard]] auto Parser::parseInterface() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::INTERFACE) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: INTERFACE"));
        }

        InterfaceDef iface = InterfaceDef();
        auto ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        if (current_token.type != TokenType::LBRACE) {
            return std::unexpected(Error(ErrType::Parser, "No body found for interface"));
        }

        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        while (current_token.type != TokenType::RBRACE) {
            if (current_token.type != TokenType::LET) {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found. Expected: LET"));
            }

            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
            std::string name = std::string(current_token.toString(L.src));

            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }

            if (current_token.type == TokenType::COLON) {
                auto attr_ret = parseInterfaceAttribute(iface, name);
                if (attr_ret.has_value()) { return std::unexpected(attr_ret.value()); }
            } else if (current_token.type == TokenType::EQUAL) {
                auto method_ret = parseInterfaceMethod(iface, name);
                if (method_ret.has_value()) { return std::unexpected(method_ret.value()); }
            } else {
                return std::unexpected(
                    Error(ErrType::Parser, "Incorrect token found in interface member"));
            }

            ret = next();
            if (ret.has_value()) { return std::unexpected(ret.value()); }
        }

        return ast.makeInterface(iface);
    }

    [[nodiscard]] auto Parser::parseInterfaceAttribute(InterfaceDef& iface, const std::string& name)
        -> std::optional<Error> {
        auto ret = next();  // Advance to type
        if (ret.has_value()) { return ret.value(); }

        if (current_token.type != TokenType::TYPE_LITERAL &&
            current_token.type != TokenType::IDENT) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: TYPE_LITERAL");
        }

        std::string type = std::string(current_token.toString(L.src));
        iface.attributes.emplace_back(name, type);

        ret = next();
        if (ret.has_value()) { return ret.value(); }

        if (current_token.type != TokenType::SEMICOLON) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: SEMICOLON");
        }

        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseInterfaceMethod(InterfaceDef& iface, const std::string& name)
        -> std::optional<Error> {
        auto ret = next();  // Advance to func keyword
        if (ret.has_value()) { return ret.value(); }

        if (current_token.type != TokenType::FUNC) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: FUNC");
        }

        FuncDef method = FuncDef();
        ret = next();  // Advance to opening paren
        if (ret.has_value()) { return ret.value(); }

        if (current_token.type != TokenType::LPAREN) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: LPAREN");
        }

        ret = next();  // Advance to first parameter or closing paren
        if (ret.has_value()) { return ret.value(); }

        while (current_token.type != TokenType::RPAREN) {
            if (current_token.type != TokenType::IDENT) {
                return Error(ErrType::Parser, "Incorrect token found. Expected: IDENT");
            }

            std::string param_name = std::string(current_token.toString(L.src));
            ret = next();  // Advance to colon
            if (ret.has_value()) { return ret.value(); }

            if (current_token.type != TokenType::COLON) {
                return Error(ErrType::Parser, "Incorrect token found. Expected: COLON");
            }

            ret = next();  // Advance to parameter type
            if (ret.has_value()) { return ret.value(); }

            if (current_token.type != TokenType::TYPE_LITERAL &&
                current_token.type != TokenType::IDENT) {
                return Error(ErrType::Parser, "Incorrect token found. Expected: TYPE_LITERAL");
            }

            std::string param_type = std::string(current_token.toString(L.src));
            method.args.emplace_back(param_name, param_type);

            ret = next();
            if (ret.has_value()) { return ret.value(); }

            if (current_token.type == TokenType::COMMA) {
                ret = next();
                if (ret.has_value()) { return ret.value(); }
            }
        }

        ret = next();  // Advance to return type
        if (ret.has_value()) { return ret.value(); }

        if (current_token.type != TokenType::TYPE_LITERAL &&
            current_token.type != TokenType::IDENT) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: TYPE_LITERAL");
        }

        method.return_type = std::string(current_token.toString(L.src));
        iface.methods.emplace_back(name, method);

        ret = next();
        if (ret.has_value()) { return ret.value(); }

        if (current_token.type != TokenType::SEMICOLON) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: SEMICOLON");
        }

        return std::nullopt;
    }

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
        std::size_t inner_id = 0;

        if (keyword == "class"sv) {
            std::expected<std::size_t, Error> e = parseClass();
            if (!e.has_value()) { return e.error(); }
            inner_id = e.value();
        } else if (keyword == "enum"sv) {
            std::expected<std::size_t, Error> e = parseEnum();
            if (!e.has_value()) { return e.error(); }
            inner_id = e.value();
        } else if (keyword == "interface"sv) {
            std::expected<std::size_t, Error> e = parseInterface();
            if (!e.has_value()) { return e.error(); }
            inner_id = e.value();
        } else {
            return Error(ErrType::Parser, "Incorrect keyword found in type");
        }

        if (current_token.type != TokenType::SEMICOLON) {
            return Error(ErrType::Parser, "Incorrect token found. Expected: SEMICOLON");
        }

        ast.makeTypeDef(std::string(name), inner_id);
        return std::nullopt;
    }

    [[nodiscard]] auto Parser::parseLet() -> std::expected<std::size_t, Error> {
        if (current_token.type != TokenType::LET) {
            return std::unexpected(Error(ErrType::Parser, "Incorrect token found. Expected: LET"));
        }

        auto ret = next();  // advance to name;
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        const std::string_view name = current_token.toString(L.src);

        ret = next();  // Advance to eq;
        if (ret.has_value()) { return std::unexpected(ret.value()); }
        if (current_token.type != TokenType::EQUAL) {
            return std::unexpected(
                Error(ErrType::Parser, "Incorrect token found. Expected: EQUAL"));
        }

        ret = next();
        if (ret.has_value()) { return std::unexpected(ret.value()); }

        std::size_t idx = 0;
        if (current_token.type == TokenType::FUNC) {
            auto func_ret = parseFunction();
            if (!func_ret.has_value()) { return std::unexpected(func_ret.error()); }
            idx = func_ret.value();
        }

        return ast.makeLet(std::string(name), idx);
    }

}  // namespace Winter
