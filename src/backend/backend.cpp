#include "backend.h"

#include <llvm/IR/Module.h>

void compileModule() {
    llvm::LLVMContext ctx;
    llvm::Module myModule("Main", ctx);
}
