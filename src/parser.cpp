#include "parser.h"

namespace Winter {
    [[nodiscard]] auto Parser::parseAlias() -> aliasNode {}
    [[nodiscard]] auto Parser::parseMod(moduleNode* mod) -> void {}
    [[nodiscard]] auto Parser::parseType() -> typeNode {}
    [[nodiscard]] auto Parser::parseLet() -> letNode {}

    [[nodiscard]] auto Parser::parse() -> moduleNode {
        moduleNode mod = {};

        Token tok;
        while (tok.type != TokenType::ENDOFFILE) {
            tok = L();
            switch (tok.type) {
                case TokenType::ALIAS: mod.contents.push_back(parseAlias()); break;
                case TokenType::MOD:   mod.contents.push_back(parseMod(&mod)); break;
                case TokenType::TYPE:  mod.contents.push_back(parseType()); break;
                case TokenType::LET:   mod.contents.push_back(parseLet()); break;
            };
        };
    }
}  // namespace Winter
