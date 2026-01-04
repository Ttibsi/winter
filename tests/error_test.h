#include <willow/test.h>

#include "error.h"

constexpr int test_errConstructor([[maybe_unused]] Willow::Test* test) {
    const auto type = Winter::ErrType::NotImplementedError;
    const auto err = Winter::Err(type, "TODO: not implemented");

    if (err.type != type) { return 1; }
    if (err.msg != "TODO: not implemented") { return 2; }

    return 0;
}

constexpr int test_getErrType([[maybe_unused]] Willow::Test* test) {
    const auto type = Winter::ErrType::NotImplementedError;
    const auto err = Winter::Err(type, "TODO: not implemented");

    if (err.getErrType() != "NotImplementedError") { return 1; }
    return 0;
}

constexpr int test_display([[maybe_unused]] Willow::Test* test) {
    const auto type = Winter::ErrType::NotImplementedError;
    const auto err = Winter::Err(type, "TODO: not implemented");
    const std::string expected = "NotImplementedError: TODO: not implemented";

    if (err.display() != expected) {
        test->alert(err.display());
        return 1;
    }

    return 0;
}
