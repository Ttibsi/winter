#include "lexer.h"

#include <cassert>
#include <cctype>
#include <ranges>
#include <string>
#include <string_view>

#include "error.h"

namespace Winter {
    [[nodiscard]] std::string Token::toString() const {
        switch (type) {
            case TokenType::LEFT_PAREN:
                return "TokenType::LEFT_PAREN  "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::RIGHT_PAREN:
                return "TokenType::RIGHT_PAREN "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::LEFT_BRACE:
                return "TokenType::LEFT_BRACE  "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::RIGHT_BRACE:
                return "TokenType::RIGHT_BRACE "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::COMMA:
                return "TokenType::COMMA       "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::DOT:
                return "TokenType::DOT         "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::SEMICOLON:
                return "TokenType::SEMICOLON   "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::COLON:
                return "TokenType::COLON       "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::MINUS:
                return "TokenType::MINUS       "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::PLUS:
                return "TokenType::PLUS        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::STAR:
                return "TokenType::STAR        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::SLASH:
                return "TokenType::SLASH       "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::BANG:
                return "TokenType::BANG        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::BANG_EQ:
                return "TokenType::BANG_EQ     "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::EQUAL:
                return "TokenType::EQUAL       "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::EQUAL_EQ:
                return "TokenType::EQUAL_EQ    "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::GREATER:
                return "TokenType::GREATER     "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::GREATER_EQ:
                return "TokenType::GREATER_EQ  "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::LESS:
                return "TokenType::LESS        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::LESS_EQ:
                return "TokenType::LESS_EQ     "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::ELLIPSIS:
                return "TokenType::ELLIPSIS    "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::AND:
                return "TokenType::AND         "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::OR:
                return "TokenType::OR          "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::CASE:
                return "TokenType::CASE        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::CLASS:
                return "TokenType::CLASS       "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::CONST:
                return "TokenType::CONST       "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::DEFAULT:
                return "TokenType::DEFAULT     "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::ELSE:
                return "TokenType::ELSE        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::ENUM:
                return "TokenType::ENUM        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::EXPORT:
                return "TokenType::EXPORT      "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::FALSE:
                return "TokenType::FALSE       "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::FOR:
                return "TokenType::FOR         "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::FUNC:
                return "TokenType::FUNC        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::IF:
                return "TokenType::IF          "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::IMPORT:
                return "TokenType::IMPORT      "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::NIL:
                return "TokenType::NIL         "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::RETURN:
                return "TokenType::RETURN      "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::STATIC:
                return "TokenType::STATIC      "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::SWITCH:
                return "TokenType::SWITCH      "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::TRUE:
                return "TokenType::TRUE        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::VAR:
                return "TokenType::VAR         "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::IDENT:
                return "TokenType::IDENT       "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::STRING:
                return "TokenType::STRING      "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::CHAR:
                return "TokenType::CHAR        "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::NUMBER:
                return "TokenType::NUMBER      "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
            case TokenType::END:
                return "TokenType::END         "
                       "Start: " +
                       std::to_string(start) + " Len: " + std::to_string(len);
        }

        return "";
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
            return 0;
        }

        assert(sv.at(0) == '.');
        if (sv.at(1) == '.' && sv.at(2) == '.') {
            makeToken(TokenType::ELLIPSIS, start, 3);
            return 3;
        }

        makeToken(TokenType::DOT, start, 1);
        return 1;
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

            switch (c) {
                case '(':
                    makeToken(TokenType::LEFT_PAREN, idx, 1);
                    break;
                case ')':
                    makeToken(TokenType::RIGHT_PAREN, idx, 1);
                    break;
                case '{':
                    makeToken(TokenType::LEFT_BRACE, idx, 1);
                    break;
                case '}':
                    makeToken(TokenType::RIGHT_BRACE, idx, 1);
                    break;
                case ',':
                    makeToken(TokenType::COMMA, idx, 1);
                    break;
                case '.':
                    advance_count = scanEllipsis(idx);
                    break;
                case ';':
                    makeToken(TokenType::SEMICOLON, idx, 1);
                    break;
                case ':':
                    makeToken(TokenType::COLON, idx, 1);
                    break;
                case '-':
                    makeToken(TokenType::MINUS, idx, 1);
                    break;
                case '+':
                    makeToken(TokenType::PLUS, idx, 1);
                    break;
                case '*':
                    makeToken(TokenType::STAR, idx, 1);
                    break;
                case '/':
                    makeToken(TokenType::SLASH, idx, 1);
                    break;
                case '\'':
                    advance_count = scanChar(idx);
                    break;
                case '"':
                    advance_count = scanStringLiteral(idx);
                    break;

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
                    if (raw_text.at(idx - 1) == '&') {
                        break;
                    } else if (raw_text.at(idx + 1) == '&') {
                        makeToken(TokenType::AND, idx, 2);
                    } else {
                        return std::unexpected(
                            Err(Err::ErrType::TokenizingError, "Singleton ampersand char found"));
                    }
                } break;

                case '|': {
                    if (raw_text.at(idx - 1) == '|') {
                        break;
                    } else if (raw_text.at(idx + 1) == '|') {
                        makeToken(TokenType::OR, idx, 2);
                    } else {
                        return std::unexpected(
                            Err(Err::ErrType::TokenizingError, "Singleton pipe char found"));
                    }
                } break;

                default:
                    break;
            };

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
}  // namespace Winter
