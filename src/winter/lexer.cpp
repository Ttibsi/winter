#include "lexer.h"

#include <cctype>
#include <ranges>
#include <string>
#include <string_view>

#include "error.h"

namespace Winter {
    void Lexer::makeToken(TokenType type, std::size_t start, std::size_t len) {
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

    std::expected<void, Err> Lexer::tokenize() {
        int advance_count = 0;

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
                    makeToken(TokenType::DOT, idx, 1);
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
