#include "frontend/parser.h"

namespace Winter {
    [[nodiscard]] bool Parser::check(const TokenType& type) const noexcept {
        return current.type == type;
    }

    void Parser::consume() noexcept {
        prev = current;
        current = L();
    }

    [[nodiscard]] std::expected<Stmt, Error> Parser::parseStmt() noexcept {}

    [[nodiscard]] std::expected<std::vector<Stmt>, Error> Parser::operator()() {
        std::vector<Stmt> code = {};

        while (!check(TokenType::eof)) {
            std::expected<Stmt, Error> expected = parseStmt();
            if (!expected.has_value()) { return std::unexpected(expected.error()); }

            code.push_back(expected.value());
        }

        return code;
    }
}  // namespace Winter
