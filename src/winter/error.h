#ifndef WINTER_ERROR_H
#define WINTER_ERROR_H

#include <expected>
#include <string>
#include <utility>

namespace Winter {
    enum class ErrType {
        NotImplementedError,
        RuntimeError,
        NameError,
        TokenizingError,
        ParsingError
    };

    struct Err {
        ErrType type;
        std::string msg;

        explicit Err(ErrType _type, std::string _msg) : type(_type), msg(_msg) {}

        [[nodiscard]] std::string getErrType() const {
            switch (type) {
                    // clang-format off
                case ErrType::NotImplementedError: return "NotImplementedError";
                case ErrType::RuntimeError: return "RuntimeError";
                case ErrType::NameError: return "NameError";
                case ErrType::TokenizingError: return "TokenizingError";
                case ErrType::ParsingError: return "ParsingError";
                    // clang-format on
            };
            std::unreachable();
        }

        [[nodiscard]] std::string display() const { return getErrType() + ": " + msg; }
    };

    using retcode_t = std::expected<int, Err>;
    using result_t = std::expected<void, Err>;
}  // namespace Winter

#endif  // WINTER_ERROR_H
