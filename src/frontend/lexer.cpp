#include "lexer.h"

#include <algorithm>
#include <array>
#include <cassert>

#include "../error.h"

namespace Winter {
    [[nodiscard]] bool between(int min, int max, int val) noexcept {
        return (min <= val && val <= max);
    }

    void Lexer::skipWhitespace() noexcept {
        static constexpr std::array<char, 3> whitespace = {' ', '\n', '\t'};
        while (playhead < src.size()) {
            const char c = src[playhead];
            const bool is_ws =
                std::any_of(whitespace.begin(), whitespace.end(), [c](char w) { return w == c; });
            if (!is_ws) { break; }
            playhead++;
        }
    }

    void Lexer::skipComment() noexcept {
        auto loc = src.find_first_of('\n', playhead);
        if (loc == std::string_view::npos) {
            playhead = src.size();
            return;
        }

        playhead = loc;
        return;
    }

    [[nodiscard]] bool Lexer::isNumeric() noexcept {
        if (playhead >= src.size()) { return false; }
        static constexpr std::array<char, 11> digits = {'0', '1', '2', '3', '4', '5',
                                                        '6', '7', '8', '9', '.'};

        auto cmp = [&](const char c) { return c == src.at(playhead); };
        return std::any_of(digits.begin(), digits.end(), cmp);
    }

    [[nodiscard]] bool Lexer::isLetter() noexcept {
        if (playhead >= src.size()) { return false; }
        char c = src.at(playhead);
        // [A-Za-z0-9_]
        return (between(65, 90, c) || between(97, 122, c) || between(48, 57, c) || c == '_');
    }

    [[nodiscard]] std::expected<Token, Error> Lexer::lexSingle(TokenType type) noexcept {
        playhead++;
        return Token(type, playhead - 1, 1);
    }

    [[nodiscard]] std::expected<Token, Error>
    Lexer::lexDouble(char c, TokenType single, TokenType pair) {
        playhead++;
        if (src.at(playhead) == c) {
            playhead++;
            return Token(pair, playhead - 2, 2);
        }
        return Token(single, playhead - 1, 1);
    }

    [[nodiscard]] std::expected<Token, Error> Lexer::lexChar() {
        playhead += 2;
        if (playhead >= src.size() || src.at(playhead) != '\'') {
            return std::unexpected(
                Error(ErrType::Lexer, std::format("Malformed char at pos {}", playhead)));
        }

        playhead++;
        return Token(TokenType::char_literal, playhead - 3, 3);
    }

    [[nodiscard]] std::expected<Token, Error> Lexer::lexString() {
        if (src.at(playhead) != '"') {
            return std::unexpected(
                Error(ErrType::Lexer, "Parsing string started at invalid location"));
        }

        std::size_t strlen = 1;
        playhead++;
        // TODO: handle escaped quotes
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

        return Token(TokenType::str_literal, playhead - strlen, strlen);
    }

    [[nodiscard]] std::expected<Token, Error> Lexer::lexNumeric() {
        Token t = Token(TokenType::num_literal, playhead);
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

    [[nodiscard]] std::expected<Token, Error> Lexer::lexIdentKeyword() {
        const std::size_t start = playhead;
        while (isLetter()) { playhead++; }

        TokenType type = TokenType::ident;
        if (keywords.contains(src.substr(start, playhead - start))) {
            type = keywords.at(src.substr(start, playhead - start));
        }

        if (types.contains(src.substr(start, playhead - start))) { type = TokenType::type_literal; }

        return Token(type, start, playhead - start);
    }

    [[nodiscard]] std::expected<Token, Error> Lexer::operator()() {
        if (playhead >= src.size()) { return Token(TokenType::eof, 0); }
        skipWhitespace();

        while (playhead < src.size() && src.at(playhead) == '#') {
            skipComment();
            skipWhitespace();
        }

        if (playhead >= src.size()) { return Token(TokenType::eof, 0); }
        switch (src.at(playhead)) {
            case '(':  return lexSingle(TokenType::lparen);
            case ')':  return lexSingle(TokenType::rparen);
            case '{':  return lexSingle(TokenType::lbrace);
            case '}':  return lexSingle(TokenType::rbrace);
            case '[':  return lexSingle(TokenType::lsquacket);
            case ']':  return lexSingle(TokenType::rsquacket);
            case ':':  return lexSingle(TokenType::colon);
            case ';':  return lexSingle(TokenType::semicolon);
            case '*':  return lexSingle(TokenType::star);
            case '/':  return lexSingle(TokenType::slash);
            case ',':  return lexSingle(TokenType::comma);
            case '+':  return lexDouble('+', TokenType::plus, TokenType::plus_plus);
            case '-':  return lexDouble('-', TokenType::minus, TokenType::minus_minus);
            case '.':  return lexDouble('.', TokenType::dot, TokenType::dot_dot);
            case '>':  return lexDouble('=', TokenType::op_greater, TokenType::op_greater_eq);
            case '<':  return lexDouble('=', TokenType::op_less, TokenType::op_less_eq);
            case '=':  return lexDouble('=', TokenType::op_equal, TokenType::op_equal_eq);
            case '!':  return lexDouble('=', TokenType::op_not, TokenType::op_not_eq);
            case '&':  return lexDouble('&', TokenType::error, TokenType::op_and);
            case '|':  return lexDouble('|', TokenType::error, TokenType::op_or);
            case '\'': return lexChar();
            case '"':  return lexString();
            default:   break;
        };

        if (isNumeric()) { return lexNumeric(); }
        if (isLetter()) { return lexIdentKeyword(); }

        return std::unexpected(
            Error(ErrType::Lexer, std::format("Invalid token found at {}", playhead)));
    }
}  // namespace Winter
