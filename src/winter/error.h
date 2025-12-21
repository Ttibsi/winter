#ifndef WINTER_ERROR_H
#define WINTER_ERROR_H

#include <expected>
#include <string>

namespace Winter {
    enum class ErrType {
        NotImplementedError,
        RuntimeError,
        NameError,
        TokenizingError,
        ParsingError,
    };

    struct Err {
        ErrType type;
        std::string msg;

        explicit Err(ErrType _type, std::string _msg) : type(_type), msg(_msg) {}
    };

    using retcode_t = std::expected<int, Err>;
    using result_t = std::expected<void, Err>;
}  // namespace Winter

#endif  // WINTER_ERROR_H
