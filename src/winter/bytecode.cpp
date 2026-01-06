#include "bytecode.h"

namespace Winter {

    [[nodiscard]] std::expected<Module, Err> Generator::generate() {
        Module mod = Module("main");

        return mod;
    }

}  // namespace Winter
