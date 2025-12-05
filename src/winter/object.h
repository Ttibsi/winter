#ifndef WINTER_OBJECT_H
#define WINTER_OBJECT_H

#include <any>
#include <cstdint>
#include <typeinfo>

namespace Winter {
    enum class ObjType {
        Null,
    };

    struct Object {
        ObjType type;
        std::any value;

        [[nodiscard]] explicit constexpr Object(ObjType _type, std::any val)
            : type(_type), value(val) {}

        template <typename T>
        T unwrap() {
            return std::any_cast<T>(value);
        }
    };
};  // namespace Winter

#endif  // WINTER_OBJECT_H
