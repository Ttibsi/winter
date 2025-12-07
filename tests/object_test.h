#include <print>
#include <string_view>

#include "winter/object.h"
using namespace std::literals;

constexpr int test_objectConstructor([[maybe_unused]] Willow::Test* test) {
    const auto o = Winter::Object(Winter::ObjType::Null, 5);

    if (o.type != Winter::ObjType::Null) {
        return 1;
    }

    if (o.value.type().name() != "i"sv) {
        return 2;
    }

    if (std::any_cast<int>(o.value) != 5) {
        return 3;
    }

    return 0;
}

constexpr int test_objectUnwrap([[maybe_unused]] Willow::Test* test) {
    auto o = Winter::Object(Winter::ObjType::Null, 5);

    if (o.value.type().name() != "i"sv) {
        return 1;
    }

    if (o.unwrap<int>() != 5) {
        return 2;
    }

    return 0;
}
