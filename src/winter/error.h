#ifndef WINTER_ERROR_H
#define WINTER_ERROR_H

#include <expected>
#include <string>

namespace Winter {
    // TODO: Error messages
    struct Err {
        enum class ErrType {
            NotImplementedError,
            RuntimeError,
            NameError,
            TokenizingError,
        };

        ErrType type;
        std::string msg;

        explicit Err(ErrType _type, std::string _msg) : type(_type), msg(_msg) {}
    };

    using retcode_t = std::expected<int, Err>;
}  // namespace Winter

#endif  // WINTER_ERROR_H
