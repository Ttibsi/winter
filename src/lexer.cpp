#include "lexer.h"

#include <algorithm>
#include <array>
#include <format>

namespace Winter {
    auto Lexer::skipWhitespace() -> void {
        static constexpr std::array<char, 3> whitespace = {' ', '\n', '\t'};
        auto cmp = [&](const char c) { return c == src.at(playhead); };
        while (std::any_of(whitespace.begin(), whitespace.end(), cmp)) { playhead++; }
    }

    // TODO: utility?
    [[nodiscard]] auto between(int min, int max, int val) -> bool {
        return (min <= val && val <= max);
    }

    [[nodiscard]] auto Lexer::isNumeric() -> bool {
        if (playhead >= src.size()) { return false; }
        static constexpr std::array<char, 11> digits = {'0', '1', '2', '3', '4', '5',
                                                        '6', '7', '8', '9', '.'};

        auto cmp = [&](const char c) { return c == src.at(playhead); };
        return std::any_of(digits.begin(), digits.end(), cmp);
    }

    [[nodiscard]] auto Lexer::isLetter() -> bool {
        if (playhead >= src.size()) { return false; }
        char c = src.at(playhead);
        // [A-Za-z0-9_]
        return (between(65, 90, c) || between(97, 122, c) || between(48, 57, c) || c == '_');
    }

    [[nodiscard]] auto Lexer::lexNumeric() -> token_result_t {
        Token t = Token(TokenType::NUM_LITERAL, playhead);
        while (isNumeric()) {
            t.len++;
            playhead++;
            if (playhead >= src.size()) { break; }
        }

        if (t.len == 0) {
            return std::unexpected(
                Error(ErrType::Lexer, std::format("Invalid numeric found at {}", playhead)));
        }

        return t;
    }

    [[nodiscard]] auto Lexer::lexSingle(const TokenType type) -> token_result_t {
        playhead++;
        return Token(type, playhead - 1, 1);
    }

    [[nodiscard]] auto Lexer::lexDouble(const char c1, const TokenType single, const TokenType pair)
        -> token_result_t {
        playhead++;
        if (src.at(playhead) == c1) {
            playhead++;
            return Token(pair, playhead - 2, 2);
        }
        return Token(single, playhead - 1, 1);
    }

    [[nodiscard]] auto Lexer::lexChar() -> token_result_t {
        playhead += 2;
        if (playhead >= src.size() || src.at(playhead) != '\'') {
            return std::unexpected(
                Error(ErrType::Lexer, std::format("Malformed char at pos {}", playhead)));
        }

        playhead++;
        return Token(TokenType::CHAR_LITERAL, playhead - 3, 3);
    }

    [[nodiscard]] auto Lexer::lexString() -> token_result_t {
        if (src.at(playhead) != '"') {
            return std::unexpected(
                Error(ErrType::Lexer, "Parsing string started at invalid location"));
        }

        std::size_t strlen = 1;
        playhead++;
        while (src.at(playhead) != '"') {
            if (playhead >= src.size()) { break; }

            strlen++;
            playhead++;

            if (playhead > src.size()) {
                return std::unexpected(Error(ErrType::Lexer, "Unclosed string"));
            }
        }

        // Include the closing quote
        strlen++;
        playhead++;

        return Token(TokenType::STR_LITERAL, playhead - strlen, strlen);
    }

    [[nodiscard]] auto Lexer::lexIdentKeyword() -> std::expected<Token, Error> {
        const std::size_t start = playhead;
        while (isLetter()) { playhead++; }

        TokenType type = TokenType::IDENT;
        if (keywords.contains(src.substr(start, playhead - start))) {
            type = keywords.at(src.substr(start, playhead - start));
        }

        if (types.contains(src.substr(start, playhead - start))) { type = TokenType::TYPE_LITERAL; }

        return Token(type, start, playhead - start);
    }

    [[nodiscard]] const auto Lexer::operator()(std::string_view source) -> token_result_t {
        src = source;
        skipWhitespace();

        switch (src.at(playhead)) {
            case '(':
                return lexSingle(TokenType::LPAREN);
            case ')':
                return lexSingle(TokenType::RPAREN);
            case '{':
                return lexSingle(TokenType::LBRACE);
            case '}':
                return lexSingle(TokenType::RBRACE);
            case '[':
                return lexSingle(TokenType::LSQUACKET);
            case ']':
                return lexSingle(TokenType::RSQUACKET);
            case ':':
                return lexSingle(TokenType::COLON);
            case ';':
                return lexSingle(TokenType::SEMICOLON);
            case '+':
                return lexSingle(TokenType::PLUS);
            case '-':
                return lexSingle(TokenType::MINUS);
            case '*':
                return lexSingle(TokenType::STAR);
            case '/':
                return lexSingle(TokenType::SLASH);
            case ',':
                return lexSingle(TokenType::COMMA);
            case '.':
                return lexDouble('.', TokenType::DOT, TokenType::DOT_DOT);
            case '>':
                return lexDouble('=', TokenType::GREATER, TokenType::GREATER_EQ);
            case '<':
                return lexDouble('=', TokenType::LESS, TokenType::LESS_EQ);
            case '!':
                return lexDouble('=', TokenType::NOT, TokenType::NOT_EQ);
            case '&':
                return lexDouble('&', TokenType::ERROR, TokenType::AND);
            case '|':
                return lexDouble('|', TokenType::ERROR, TokenType::OR);
            case '\'':
                return lexChar();
            case '"':
                return lexString();
            default:
                break;
        }

        if (isNumeric()) { return lexNumeric(); }
        if (isLetter()) { return lexIdentKeyword(); }

        return std::unexpected(
            Error(ErrType::Lexer, std::format("Invalid token found at {}", playhead)));
    }
};  // namespace Winter
