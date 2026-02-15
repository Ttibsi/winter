#ifndef WINTER_ERROR_H
#define WINTER_ERROR_H

#include <cstdint>
#include <expected>
#include <string>

namespace Winter {
    // pre-defined
    struct Token;

    enum class ErrType : uint8_t {
        Lexer,
        NotImplemented,
        Parser
    };

    struct Error {
        ErrType type;
        std::string msg;

        explicit constexpr Error(ErrType t, std::string m) : type(t), msg(m) {}
    };

    using token_result_t = std::expected<Token, Error>;

}  // namespace Winter

#endif  // WINTER_ERROR_H
