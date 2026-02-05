#include <cstddef>

namespace Winter {
    enum class TokenType {};

    struct Token {
        TokenType type;
        std::size_t start;
        std::size_t len;

        explicit Token(TokenType typ, std::size_t s, std::size_t l) : type(typ), start(s), len(l) {}
        auto operator<=>(const Token&) const = default;
        [[nodiscard]] std::string toString() const;
    };
}  // namespace Winter
