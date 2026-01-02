#include "lexer.h"

#include <cassert>
#include <cctype>
#include <format>
#include <ranges>
#include <string>
#include <string_view>

#include "error.h"

namespace Winter {
    [[nodiscard]] std::string Token::toString() const {
        std::string tok = "";

        // clang-format off
        switch (type) {
            case TokenType::LEFT_PAREN: tok = "TokenType::LEFT_PAREN"; break;
            case TokenType::RIGHT_PAREN: tok = "TokenType::RIGHT_PAREN"; break;
            case TokenType::LEFT_BRACE: tok = "TokenType::LEFT_BRACE"; break;
            case TokenType::RIGHT_BRACE: tok = "TokenType::RIGHT_BRACE"; break;
            case TokenType::COMMA: tok = "TokenType::COMMA"; break;
            case TokenType::DOT: tok = "TokenType::DOT"; break;
            case TokenType::SEMICOLON: tok = "TokenType::SEMICOLON"; break;
            case TokenType::COLON: tok = "TokenType::COLON"; break;
            case TokenType::MINUS: tok = "TokenType::MINUS"; break;
            case TokenType::PLUS: tok = "TokenType::PLUS"; break;
            case TokenType::STAR: tok = "TokenType::STAR"; break;
            case TokenType::SLASH: tok = "TokenType::SLASH"; break;
            case TokenType::BANG: tok = "TokenType::BANG"; break;
            case TokenType::BANG_EQ: tok = "TokenType::BANG_EQ"; break;
            case TokenType::EQUAL: tok = "TokenType::EQUAL"; break;
            case TokenType::EQUAL_EQ: tok = "TokenType::EQUAL_EQ"; break;
            case TokenType::GREATER: tok = "TokenType::GREATER"; break;
            case TokenType::GREATER_EQ: tok = "TokenType::GREATER_EQ"; break;
            case TokenType::LESS: tok = "TokenType::LESS"; break;
            case TokenType::LESS_EQ: tok = "TokenType::LESS_EQ"; break;
            case TokenType::ELLIPSIS: tok = "TokenType::ELLIPSIS"; break;
            case TokenType::AND: tok = "TokenType::AND"; break;
            case TokenType::OR: tok = "TokenType::OR"; break;
            case TokenType::CASE: tok = "TokenType::CASE"; break;
            case TokenType::CLASS: tok = "TokenType::CLASS"; break;
            case TokenType::CONST: tok = "TokenType::CONST"; break;
            case TokenType::DEFAULT: tok = "TokenType::DEFAULT"; break;
            case TokenType::ELSE: tok = "TokenType::ELSE"; break;
            case TokenType::ENUM: tok = "TokenType::ENUM"; break;
            case TokenType::EXPORT: tok = "TokenType::EXPORT"; break;
            case TokenType::FALSE: tok = "TokenType::FALSE"; break;
            case TokenType::FOR: tok = "TokenType::FOR"; break;
            case TokenType::FUNC: tok = "TokenType::FUNC"; break;
            case TokenType::IF: tok = "TokenType::IF"; break;
            case TokenType::IMPORT: tok = "TokenType::IMPORT"; break;
            case TokenType::NIL: tok = "TokenType::NIL"; break;
            case TokenType::RETURN: tok = "TokenType::RETURN"; break;
            case TokenType::STATIC: tok = "TokenType::STATIC"; break;
            case TokenType::SWITCH: tok = "TokenType::SWITCH"; break;
            case TokenType::TRUE: tok = "TokenType::TRUE"; break;
            case TokenType::VAR: tok = "TokenType::VAR"; break;
            case TokenType::IDENT: tok = "TokenType::IDENT"; break;
            case TokenType::STRING: tok = "TokenType::STRING"; break;
            case TokenType::CHAR: tok = "TokenType::CHAR"; break;
            case TokenType::NUMBER: tok = "TokenType::NUMBER"; break;
            case TokenType::END: tok = "TokenType::END"; break;
        }

        // clang-format on
        return std::format("{:<22} Start: {}, Len: {}", tok, start, len);
    }

    void Lexer::makeToken(TokenType type, long start, std::size_t len) {
        tokens.emplace_back(type, start, len);
    }

    [[nodiscard]] std::size_t Lexer::scanNumber(std::size_t start) {
        auto sv = std::string_view(raw_text.begin() + start, raw_text.end());
        std::size_t pos = 0;
        std::string buf = "";

        while (std::isdigit(sv.at(pos))) {
            pos++;
            if (pos == sv.size()) {
                break;
            }
        }
        makeToken(TokenType::NUMBER, start, pos);
        return pos;
    }

    [[nodiscard]] std::size_t Lexer::scanStringLiteral(std::size_t start) {
        auto sv = std::string_view(raw_text.begin() + start, raw_text.end());

        // starting at +1 because 0 should be opening quote
        std::size_t pos = 1;
        std::string buf = "";

        while (sv.at(pos) != '"') {
            pos++;
        }
        pos++;  // closing quote
        makeToken(TokenType::STRING, start, pos);
        return pos;
    }

    [[nodiscard]] std::size_t Lexer::scanChar(std::size_t start) {
        auto sv = std::string_view(raw_text.begin() + start, raw_text.end());
        assert(sv.at(2) == '\'');  // We have an opening/closing pair of single quotes

        makeToken(TokenType::CHAR, start, 3);
        return 3;
    }

    [[nodiscard]] bool Lexer::validIdentChar(char c) const {
        if (c == '_') {
            return true;
        }
        if (std::isalnum(c)) {
            return true;
        }
        return false;
    }

    [[nodiscard]] std::optional<std::size_t> Lexer::scanKeyword(
        std::string_view sv,
        std::size_t start) {
        for (auto&& pair : keyword_regex) {
            if (std::regex_search(sv.begin(), sv.end(), pair.regex)) {
                makeToken(pair.type, start, pair.length);
                return pair.length;
            }
        }

        return {};
    }

    [[nodiscard]] std::size_t Lexer::scanIdentifier(std::size_t start) {
        auto sv = std::string_view(raw_text.begin() + start, raw_text.end());
        if (sv.empty()) {
            return 0;
        }

        auto kw_ret = scanKeyword(sv, start);
        if (kw_ret.has_value()) {
            return kw_ret.value();
        }

        std::size_t pos = 0;
        while (validIdentChar(sv.at(pos))) {
            pos++;
            if (pos == sv.size()) {
                break;
            }
        }

        if (!pos) {
            return 0;
        }
        makeToken(TokenType::IDENT, start, pos);
        return pos;
    }

    [[nodiscard]] std::size_t Lexer::scanEllipsis(std::size_t start) {
        auto sv = std::string_view(raw_text.begin() + start, raw_text.end());
        if (sv.empty()) {
            // error?
            return 0;
        }

        assert(sv.at(0) == '.');
        if (sv.size() < 3) {
            makeToken(TokenType::DOT, start, 1);
            return 1;
        }

        if (sv.at(1) == '.' && sv.at(2) == '.') {
            makeToken(TokenType::ELLIPSIS, start, 3);
            return 3;
        }

        return 0;
    }

    [[nodiscard]] std::expected<void, Err> Lexer::tokenize() {
        std::size_t advance_count = 0;

        for (const auto&& [idx, c] : std::views::enumerate(raw_text)) {
            if (advance_count) {
                advance_count--;
                continue;
            }

            if (std::isspace(c)) {
                continue;
            }

            // clang-format off
            switch (c) {
                case '(': makeToken(TokenType::LEFT_PAREN, idx, 1); break;
                case ')': makeToken(TokenType::RIGHT_PAREN, idx, 1); break;
                case '{': makeToken(TokenType::LEFT_BRACE, idx, 1); break;
                case '}': makeToken(TokenType::RIGHT_BRACE, idx, 1); break;
                case ',': makeToken(TokenType::COMMA, idx, 1); break;
                case '.': advance_count = scanEllipsis(idx); break;
                case ';': makeToken(TokenType::SEMICOLON, idx, 1); break;
                case ':': makeToken(TokenType::COLON, idx, 1); break;
                case '-': makeToken(TokenType::MINUS, idx, 1); break;
                case '+': makeToken(TokenType::PLUS, idx, 1); break;
                case '*': makeToken(TokenType::STAR, idx, 1); break;
                case '/': makeToken(TokenType::SLASH, idx, 1); break;
                case '\'': advance_count = scanChar(idx); break;
                case '"': advance_count = scanStringLiteral(idx); break;
                case '=': {
                    if (std::string("!=<>").contains(raw_text.at(idx - 1))) {
                        switch (raw_text.at(idx - 1)) {
                            case '!':
                                makeToken(TokenType::BANG_EQ, idx - 1, 2);
                                break;
                            case '=':
                                makeToken(TokenType::EQUAL_EQ, idx - 1, 2);
                                break;
                                // case '<': makeToken(TokenType::LESS_EQ, idx - 1, 2); break;
                                // case '>': makeToken(TokenType::GREATER_EQ, idx - 1, 2); break;
                        }
                    } else if (raw_text.at(idx + 1) == '=') {
                        break;
                    } else {
                        makeToken(TokenType::EQUAL, idx, 1);
                        break;
                    }

                } break;

                case '&': {
                    if (raw_text.at(idx - 1) == '&') { break; }
                    if (raw_text.at(idx + 1) == '&') {
                        makeToken(TokenType::AND, idx, 2);
                    } else {
                        return std::unexpected(
                            Err(ErrType::TokenizingError, "Singleton ampersand char found"));
                    }
                } break;

                case '|': {
                    if (raw_text.at(idx - 1) == '|') { break; }
                    if (raw_text.at(idx + 1) == '|') {
                        makeToken(TokenType::OR, idx, 2);
                    } else {
                        return std::unexpected(
                            Err(ErrType::TokenizingError, "Singleton pipe char found"));
                    }
                } break;

                default:
                    break;
            };

            // clang-format on

            // numbers
            if (std::isdigit(c)) {
                advance_count = scanNumber(idx) - 1;
                continue;
            }

            if (std::isalnum(c)) {
                advance_count = scanIdentifier(idx) - 1;
                continue;
            }
        }

        makeToken(TokenType::END, raw_text.size(), 0);
        return {};
    }

    [[nodiscard]] std::expected<void, Err> Lexer::advance(const TokenType& tok) {
        if (playhead < tokens.size()) {
            playhead++;
        }
        if (tokens.at(playhead).type != tok) {
            return std::unexpected(Err(ErrType::ParsingError, "Wrong token found"));
        }

        return {};
    }

    void Lexer::advance() {
        if (playhead < tokens.size()) {
            playhead++;
        }
    }

    [[nodiscard]] Token* Lexer::currToken() {
        return &tokens.at(playhead);
    }

    [[nodiscard]] bool Lexer::check(const TokenType& tok) {
        return tokens.at(playhead).type == tok;
    }

    [[nodiscard]] bool Lexer::checkNext(const TokenType& tok) {
        if (playhead == tokens.size() - 1) {
            return false;
        }
        return tokens.at(playhead + 1).type == tok;
    }

    [[nodiscard]] bool Lexer::atEnd() const {
        return tokens.at(playhead).type == TokenType::END;
    }
}  // namespace Winter
