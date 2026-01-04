#include "winter/winter.h"

constexpr int test_vmPush([[maybe_unused]] Willow::Test* test) {
    auto vm = Winter::VM(false);
    vm.push(Winter::Object(Winter::ObjType::Null, 5));

    if (vm.stack.top().type != Winter::ObjType::Null) { return 1; }
    if (vm.stack.top().unwrap<int>() != 5) { return 2; }

    return 0;
}

constexpr int test_vmPop([[maybe_unused]] Willow::Test* test) {
    auto vm = Winter::VM(false);
    const auto val = vm.pop();

    if (val.has_value()) { return 1; }
    const auto rtErr = Winter::ErrType::RuntimeError;
    if (val.error().type != rtErr) { return 2; }

    vm.push(Winter::Object(Winter::ObjType::Null, 5));
    const auto val2 = vm.pop();

    if (!val2.has_value()) { return 3; }
    if (val2.value().unwrap<int>() != 5) { return 4; }
    return 0;
}

constexpr int test_vmDoString([[maybe_unused]] Willow::Test* test) {
    return 0;
}

constexpr int test_vmDoFile([[maybe_unused]] Willow::Test* test) {
    return 0;
}

constexpr int test_vmCall([[maybe_unused]] Willow::Test* test) {
    return 0;
}
