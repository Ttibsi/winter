#include "lexer.h"

namespace Winter {
    [[nodiscard]] auto Lexer::tokenize(std::string_view src) -> std::expected<void, Error> {
        skipWhitespace();

        switch (src.at(playhead)) {}
    }
};  // namespace Winter
