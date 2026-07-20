#ifndef WINTER_BACKEND_H
#define WINTER_BACKEND_H

#include <expected>
#include <optional>
#include <span>

#include "../error.h"
#include "../frontend/parser.h"

namespace Winter {
    [[nodiscard]] std::optional<Error> compileModule(std::span<Node>);
}  // namespace Winter

#endif  // WINTER_BACKEND_H
