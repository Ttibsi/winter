#ifndef WINTER_ERROR_H
#define WINTER_ERROR_H

#include <cstdint>
#include <string>

namespace Winter {
    enum class ErrType : std::uint8_t {
        Lexer,
        Parser,
        NotImplemented,
        none
    };

    struct Error {
        ErrType type;
        std::string msg;

        explicit Error(ErrType t, std::string m) : type(t), msg(m) {}
    };
}  // namespace Winter

#endif
