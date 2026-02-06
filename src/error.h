#ifndef WINTER_ERROR_H
#define WINTER_ERROR_H

namespace Winter {
    enum class ErrType: uint8_t {
        NotImplemented
    }

    struct Error {
        ErrType type;
        std::string msg;

        explicit constexpr Error(ErrType t, std::string m) : type(t), msg(m) {}
    };

}  // namespace Winter

#endif  // WINTER_ERROR_H
