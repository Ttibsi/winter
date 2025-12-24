#include "parser.h"

#include <algorithm>
#include <array>
#include <cassert>
#include <cstdlib>
#include <utility>

#include "error.h"

namespace Winter {
    [[nodiscard]] binding_t Parser::prefixBindingPower(const TokenType& tok) const {
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

    [[nodiscard]] binding_t Parser::infixBindingPower(const TokenType& tok) const {
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

    [[nodiscard]] std::expected<FunctionNode_ptr, Err> Parser::parseFunc() {
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
                    Err(ErrType::ParsingError, "Token IDENT not found in function params"));
            }

            func->params.push_back(*L->currToken());
            L->advance();
            if (L->currToken()->type == TokenType::RIGHT_PAREN) {
                break;
            } else if (L->currToken()->type == TokenType::COMMA) {
                continue;
            }
        }

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

    [[nodiscard]] std::expected<BlockNode_ptr, Err> Parser::parseBlock() {
        // TODO: Should static be in here?
        constexpr std::array finishers = {
            TokenType::CLASS, TokenType::ENUM, TokenType::EXPORT, TokenType::FUNC,
            TokenType::IMPORT};

        assert(L->currToken()->type == TokenType::LEFT_BRACE);
        L->advance();
        auto block = std::make_unique<BlockNode>();
        const auto is_finisher = [this](const TokenType& t) { return L->check(t); };
        while (std::ranges::none_of(finishers, is_finisher)) {
            auto ret = parseStatement();
            if (!ret.has_value()) {
                return std::unexpected(ret.error());
            }

            block->stmts.push_back(std::move(ret.value()));
            L->advance();
        }

        assert(L->currToken()->type == TokenType::RIGHT_BRACE);
        L->advance();
        return block;
    }

    [[nodiscard]] std::expected<ASTNode_ptr, Err> Parser::parseStatement() {
        switch (L->currToken()->type) {
            case TokenType::RETURN: {
                auto ret = parseReturn();
                if (!ret.has_value()) {
                    return std::unexpected(ret.error());
                }
                // auto ret2 = L->advance(TokenType::SEMICOLON);
                // if (!ret2.has_value()) {
                //     return std::unexpected(ret.error());
                // }

                return std::move(ret.value());
            } break;
        };

        return std::unexpected(
            Err(ErrType::ParsingError,
                "Parsing incorrect statement type: " + L->currToken()->toString()));
    }

    [[nodiscard]] std::expected<ReturnNode_ptr, Err> Parser::parseReturn() {
        assert(L->currToken()->type == TokenType::RETURN);
        auto ret = std::make_unique<ReturnNode>();
        L->advance();
        auto expr = parseExpression(0);
        if (!expr.has_value()) {
            return std::unexpected(expr.error());
        }
        ret->expr = std::move(expr.value());
        L->advance();
        return ret;
    }

    [[nodiscard]] std::expected<ExprNode_ptr, Err> Parser::parseExpression(binding_t min_bp) {
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
            return std::unexpected(
                Err(ErrType::ParsingError, "Parser should have found end of file"));
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

            std::expected<ExprNode_ptr, Err> rhs = parseExpression(bp + 1);
            if (!rhs.has_value()) {
                return std::unexpected(rhs.error());
            }

            node->op = op_type;
            node->rhs = std::move(rhs.value());
        }

        assert(L->currToken()->type == TokenType::SEMICOLON);
        return node;
    }

    [[nodiscard]] result_t Parser::parseTree() {
        while (L->playhead < L->tokens.size()) {
            switch (L->currToken()->type) {
                case TokenType::FUNC: {
                    std::expected<FunctionNode_ptr, Err> result = parseFunc();
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
