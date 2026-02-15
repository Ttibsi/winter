#include "lexer.h"

#include <algorithm>
#include <array>
#include <format>
#include <iterator>
#include <utility>

namespace Winter {
    [[nodiscard]] auto toString(const TokenType tok) -> std::string_view {
        switch (tok) {
            case TokenType::LPAREN:       return "LPAREN"sv;
            case TokenType::RPAREN:       return "RPAREN"sv;
            case TokenType::LBRACE:       return "LBRACE"sv;
            case TokenType::RBRACE:       return "RBRACE"sv;
            case TokenType::LSQUACKET:    return "LSQUACKET"sv;
            case TokenType::RSQUACKET:    return "RSQUACKET"sv;
            case TokenType::COLON:        return "COLON"sv;
            case TokenType::SEMICOLON:    return "SEMICOLON"sv;
            case TokenType::PLUS:         return "PLUS"sv;
            case TokenType::PLUS_PLUS:    return "PLUS_PLUS"sv;
            case TokenType::MINUS:        return "MINUS"sv;
            case TokenType::MINUS_MINUS:  return "MINUS_MINUS"sv;
            case TokenType::STAR:         return "STAR"sv;
            case TokenType::SLASH:        return "SLASH"sv;
            case TokenType::COMMA:        return "COMMA"sv;
            case TokenType::DOT:          return "DOT"sv;
            case TokenType::DOT_DOT:      return "DOT_DOT"sv;
            case TokenType::GREATER:      return "GREATER"sv;
            case TokenType::GREATER_EQ:   return "GREATER_EQ"sv;
            case TokenType::LESS:         return "LESS"sv;
            case TokenType::LESS_EQ:      return "LESS_EQ"sv;
            case TokenType::EQUAL:        return "EQUAL"sv;
            case TokenType::EQUAL_EQ:     return "EQUAL_EQ"sv;
            case TokenType::NOT:          return "NOT"sv;
            case TokenType::NOT_EQ:       return "NOT_EQ"sv;
            case TokenType::AND:          return "AND"sv;
            case TokenType::OR:           return "OR"sv;
            case TokenType::ALIAS:        return "ALIAS"sv;
            case TokenType::BREAK:        return "BREAK"sv;
            case TokenType::CASE:         return "CASE"sv;
            case TokenType::CLASS:        return "CLASS"sv;
            case TokenType::CONTINUE:     return "CONTINUE"sv;
            case TokenType::DEFAULT:      return "DEFAULT"sv;
            case TokenType::ENUM:         return "ENUM"sv;
            case TokenType::FALSE:        return "FALSE"sv;
            case TokenType::FOR:          return "FOR"sv;
            case TokenType::FUNC:         return "FUNC"sv;
            case TokenType::IF:           return "IF"sv;
            case TokenType::IMPLEMENTS:   return "IMPLEMENTS"sv;
            case TokenType::INTERFACE:    return "INTERFACE"sv;
            case TokenType::LET:          return "LET"sv;
            case TokenType::MOD:          return "MOD"sv;
            case TokenType::RETURN:       return "RETURN"sv;
            case TokenType::STATIC:       return "STATIC"sv;
            case TokenType::SWITCH:       return "SWITCH"sv;
            case TokenType::TRUE:         return "TRUE"sv;
            case TokenType::TYPE:         return "TYPE"sv;
            case TokenType::CHAR_LITERAL: return "CHAR_LITERAL"sv;
            case TokenType::NUM_LITERAL:  return "NUM_LITERAL"sv;
            case TokenType::STR_LITERAL:  return "STR_LITERAL"sv;
            case TokenType::TYPE_LITERAL: return "TYPE_LITERAL"sv;
            case TokenType::IDENT:        return "IDENT"sv;
            case TokenType::ERROR:        return "ERROR"sv;
        };

        std::unreachable();
    }

    auto Lexer::skipWhitespace() -> void {
        static constexpr std::array<char, 3> whitespace = {' ', '\n', '\t'};
        auto cmp = [&](const char c) { return c == src.at(playhead); };
        while (std::any_of(whitespace.begin(), whitespace.end(), cmp)) { playhead++; }
    }

    auto Lexer::skipComment() -> void {
        if (playhead >= src.size()) {
            playhead = src.size();
            return;
        }

        auto it = std::ranges::find_if(
            src.begin() + playhead, src.end(), [](char c) { return c == '\n'; });
        playhead = (it != src.end()) ? std::distance(src.begin(), it) : src.size();
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

    [[nodiscard]] auto Lexer::operator()() -> token_result_t {
        skipWhitespace();

        if (src.at(playhead) == '#') {
            skipComment();
            skipWhitespace();
        }

        switch (src.at(playhead)) {
            case '(':  return lexSingle(TokenType::LPAREN);
            case ')':  return lexSingle(TokenType::RPAREN);
            case '{':  return lexSingle(TokenType::LBRACE);
            case '}':  return lexSingle(TokenType::RBRACE);
            case '[':  return lexSingle(TokenType::LSQUACKET);
            case ']':  return lexSingle(TokenType::RSQUACKET);
            case ':':  return lexSingle(TokenType::COLON);
            case ';':  return lexSingle(TokenType::SEMICOLON);
            case '+':  return lexDouble('+', TokenType::PLUS, TokenType::PLUS_PLUS);
            case '-':  return lexDouble('-', TokenType::MINUS, TokenType::MINUS_MINUS);
            case '*':  return lexSingle(TokenType::STAR);
            case '/':  return lexSingle(TokenType::SLASH);
            case ',':  return lexSingle(TokenType::COMMA);
            case '.':  return lexDouble('.', TokenType::DOT, TokenType::DOT_DOT);
            case '>':  return lexDouble('=', TokenType::GREATER, TokenType::GREATER_EQ);
            case '<':  return lexDouble('=', TokenType::LESS, TokenType::LESS_EQ);
            case '!':  return lexDouble('=', TokenType::NOT, TokenType::NOT_EQ);
            case '&':  return lexDouble('&', TokenType::ERROR, TokenType::AND);
            case '|':  return lexDouble('|', TokenType::ERROR, TokenType::OR);
            case '\'': return lexChar();
            case '"':  return lexString();
            default:   break;
        }

        if (isNumeric()) { return lexNumeric(); }
        if (isLetter()) { return lexIdentKeyword(); }

        return std::unexpected(
            Error(ErrType::Lexer, std::format("Invalid token found at {}", playhead)));
    }
};  // namespace Winter
