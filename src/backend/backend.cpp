#include "backend.h"

#include <llvm/IR/Module.h>

namespace Winter {
    void compileModule(const Parser* P) {
        llvm::LLVMContext ctx;
        llvm::Module myModule("Main", ctx);
    }
}  // namespace Winter
