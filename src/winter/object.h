#ifndef WINTER_OBJECT_H
#define WINTER_OBJECT_H

namespace Winter {
    enum class ObjType {
        Null,
    };

    struct Object {
        ObjType type;
        uint32_t value;

        [[nodiscard]] explicit constexpr Object(ObjType _type, uint32_t val)
            : type(_type), value(val) {}
    };
};  // namespace Winter

#endif  // WINTER_OBJECT_H
