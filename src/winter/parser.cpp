#include "parser.h"

#include <cassert>

namespace Winter {
    [[nodiscard]] expected_node_t<BlockNode> Parser::parseBlock() {
        assert(L->currToken()->type == TokenType::LEFT_BRACE);

        L->advance();
        auto block = std::make_unique<BlockNode>();

        while (!L->atEnd() && !L->check(TokenType::RIGHT_BRACE)) {
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

    [[nodiscard]] expected_node_t<FuncNode> Parser::parseFunc() {
        auto ret = L->advance(TokenType::IDENT);
        if (!ret.has_value()) {
            return std::unexpected(ret.error());
        }

        auto func = std::make_unique<FuncNode>();
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

    [[nodiscard]] expected_node_t<RootNode> Parser::parse_tree() {
        std::unique_ptr<RootNode> root = std::make_unique<RootNode>();

        while (L->playhead < L->tokens.size()) {
            switch (L->currToken()->type) {
                case TokenType::FUNC: {
                    expected_node_t<FuncNode> result = parseFunc();
                    if (!result.has_value()) {
                        return std::unexpected(result.error());
                    };

                    root->children.push_back(std::move(result.value()));
                } break;
            }
        }

        return root;
    }

};  // namespace Winter
