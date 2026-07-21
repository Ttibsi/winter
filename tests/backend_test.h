#ifndef WINTER_BACKEND_TEST_H
#define WINTER_BACKEND_TEST_H

#include <willow/willow.h>

#include "backend/backend.h"

using namespace Winter;

[[nodiscard]] constexpr int test_getType([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_createFunction([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_createBlock([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_compileExpression([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_populateBlock([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

[[nodiscard]] constexpr int test_compileModule([[maybe_unused]] Willow::Test* test) noexcept {
    return 1;
}

#endif  // WINTER_BACKEND_TEST_H
