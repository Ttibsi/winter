#ifndef WINTER_BACKEND_H
#define WINTER_BACKEND_H

#include <expected>
#include <memory>
#include <optional>
#include <span>

#include <llvm/IR/Module.h>

#include "../error.h"
#include "../frontend/parser.h"

namespace Winter {
    using module_result_t = std::expected<std::unique_ptr<llvm::Module>, Error>;
    [[nodiscard]] module_result_t compileModule(std::span<Node>);
}  // namespace Winter

#endif  // WINTER_BACKEND_H
