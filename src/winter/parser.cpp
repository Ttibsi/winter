#include "parser.h"

#include <algorithm>
#include <array>
#include <cstdlib>

namespace Winter {
    [[nodiscard]] binding_t Parser::prefixBindingPower(const TokenType& tok) {
        switch (tok) {
            case TokenType::BANG:
                [[fallthrough]];
            case TokenType::MINUS:
                return 8;
            default:
                return 0;
        };

        return 0;
    }

    [[nodiscard]] binding_t Parser::infixBindingPower(const TokenType& tok) {
        switch (tok) {
            case TokenType::LEFT_PAREN:
                [[fallthrough]];
            case TokenType::RIGHT_PAREN:
                [[fallthrough]];
            case TokenType::DOT:
                return 9;

            case TokenType::STAR:
                [[fallthrough]];
            case TokenType::SLASH:
                return 7;

            case TokenType::PLUS:
                [[fallthrough]];
            case TokenType::MINUS:
                return 6;

            case TokenType::GREATER_EQ:
                [[fallthrough]];
            case TokenType::LESS_EQ:
                [[fallthrough]];
            case TokenType::GREATER:
                [[fallthrough]];
            case TokenType::LESS:
                return 5;

            case TokenType::EQUAL_EQ:
                [[fallthrough]];
            case TokenType::BANG_EQ:
                return 4;

            case TokenType::AND:
                return 3;

            case TokenType::OR:
                return 2;

            case TokenType::EQUAL:
                return 1;
            default:
                return 0;
        }

        return 0;
    }

    [[nodiscard]] std::expected<std::unique_ptr<FunctionNode>, Err> Parser::parseFunc() {
        auto ret = L->advance(TokenType::IDENT);
        if (!ret.has_value()) {
            return std::unexpected(ret.error());
        }

        auto func = std::make_unique<FunctionNode>();
        func->name = L->raw_text.substr(L->currToken()->start, L->currToken()->len);

        // params
        ret = L->advance(TokenType::LEFT_PAREN);
        if (!ret.has_value()) {
            return std::unexpected(ret.error());
        }

        while (true) {
            L->advance();
            if (L->currToken()->type == TokenType::RIGHT_PAREN) {
                break;
            } else if (L->currToken()->type != TokenType::IDENT) {
                return std::unexpected(
                    Err(Err::ErrType::ParsingError, "Token IDENT not found in function params"));
            }

            func->params.push_back(*L->currToken());
            L->advance();
            if (L->currToken()->type == TokenType::RIGHT_PAREN) {
                break;
            } else if (L->currToken()->type == TokenType::COMMA) {
                continue;
            }
        }
        // func f()
        // func f(bar)
        // func f(bar,baz)
        // func f(bar,)

        // body
        ret = L->advance(TokenType::LEFT_BRACE);
        if (!ret.has_value()) {
            return std::unexpected(ret.error());
        }

        auto block = parseBlock();
        if (!block.has_value()) {
            return std::unexpected(block.error());
        }
        func->body = std::move(block.value());

        return func;
    }

    [[nodiscard]] std::expected<std::unique_ptr<BlockNode>, Err> Parser::parseBlock() {
        // TODO: Should static be in here?
        constexpr std::array finishers = {
            TokenType::CLASS, TokenType::ENUM, TokenType::EXPORT, TokenType::FUNC,
            TokenType::IMPORT};

        L->advance();
        auto block = std::make_unique<BlockNode>();
        const auto is_finisher = [this](const TokenType& t) { return L->check(t); };
        while (std::ranges::none_of(finishers, is_finisher)) {
            auto ret = parseStatement();
            if (!ret.has_value()) {
                return std::unexpected(ret.error());
            }

            block->stmts.push_back(std::move(ret.value()));
        }

        return block;
    }

    [[nodiscard]] std::expected<std::unique_ptr<ASTNode>, Err> Parser::parseStatement() {
        switch (L->currToken()->type) {
            case TokenType::RETURN: {
                auto ret = parseReturn();
                if (!ret.has_value()) {
                    return std::unexpected(ret.error());
                }
                return std::move(ret.value());
            } break;
        };

        return std::unexpected(Err(Err::ErrType::ParsingError, "Parsing incorrect statement type"));
    }

    [[nodiscard]] std::expected<std::unique_ptr<ReturnNode>, Err> Parser::parseReturn() {
        L->advance();
        auto ret = std::make_unique<ReturnNode>();
        auto expr = parseExpression(0);
        if (!expr.has_value()) {
            return std::unexpected(expr.error());
        }
        ret->expr = std::move(expr.value());
        return ret;
    }

    [[nodiscard]] std::expected<std::unique_ptr<ExprNode>, Err> Parser::parseExpression(
        binding_t min_bp) {
        std::unique_ptr<ExprNode> node = std::make_unique<ExprNode>();
        const Token* tok = L->currToken();
        std::string_view source = L->raw_text;

        switch (tok->type) {
            case TokenType::NUMBER:
                auto num_sv =
                    std::string_view(source.begin() + tok->start, source.begin() + tok->len);
                node->lhs = std::make_unique<ValueNode>(std::atof(num_sv.data()));
                break;
        }

        if (L->checkNext(TokenType::END)) {
            // TODO: return failure
        } else if (L->checkNext(TokenType::SEMICOLON) || L->checkNext(TokenType::RIGHT_PAREN)) {
            return node;
        }

        while (true) {
            L->advance();
            const TokenType op_type = L->currToken()->type;
            if (op_type == TokenType::SEMICOLON || op_type == TokenType::RIGHT_PAREN) {
                break;
            }

            const binding_t bp = infixBindingPower(L->currToken()->type);
            if (bp < min_bp) {
                break;
            }
            L->advance();

            std::expected<std::unique_ptr<ExprNode>, Err> rhs = parseExpression(bp + 1);
            if (!rhs.has_value()) {
                return std::unexpected(rhs.error());
            }

            node->op = op_type;
            node->rhs = std::move(rhs.value());
        }

        return node;
    }

    [[nodiscard]] result_t Parser::parseTree() {
        while (L->playhead < L->tokens.size()) {
            switch (L->currToken()->type) {
                case TokenType::FUNC: {
                    std::expected<std::unique_ptr<FunctionNode>, Err> result = parseFunc();
                    if (!result.has_value()) {
                        return std::unexpected(result.error());
                    }
                    tree.push_back(std::move(result.value()));
                } break;

                default:
                    break;
            }
        }

        return {};
    }
}  // namespace Winter
