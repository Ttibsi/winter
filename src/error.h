#ifndef WINTER_ERROR_H
#define WINTER_ERROR_H

#include <cstdint>
#include <expected>
#include <string>

namespace Winter {
    enum class ErrType : std::uint8_t {
        Lexer,
        Parser,
        Generator,
        NotImplemented,
        none
    };

    struct Error {
        ErrType type;
        std::string msg;

        explicit Error(ErrType t, std::string m) : type(t), msg(m) {}
        [[nodiscard]] const static std::unexpected<Error> TODO() {
            return std::unexpected(Error(ErrType::NotImplemented, "TODO"));
        }
    };
}  // namespace Winter

#endif
