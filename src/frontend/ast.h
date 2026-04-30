#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <cstddef>
#include <memory>
#include <variant>

namespace Winter {
    // Expressions
    enum class ExprType : std::uint8_t {
        funcCall,
        binaryExpr,
        unaryExpr,
        arrayAccess,      // e.g., `arr[0]`
        castExpr,         // e.g., `i32(3.14)`
        constructorCall,  // e.g., `C[bool]{}`
        dotAccess,        // e.g., `obj.field` or `Enum.val`
        groupExpr,        // e.g., `(a + b)`
        indexExpr,        // e.g., `container[i]`
        memberAccess,     // e.g., `obj.method()`
        ternaryExpr,      // e.g., `a ? b : c`
        typeAccess,       // e.g., `MyType` in `MyType{}`
    };

    struct Expr {
        std::variant<> data;
        ExprType type;
    };

    using ExprPtr = std::unique_ptr<Expr>;

    // Declarations
    enum class DeclType : std::uint8_t {
        classDecl,
        enumDecl,
        funcDecl,
        includeDecl,
        interfaceDecl,
        varDecl,
        constDecl,         // `const let x = 5;`
        enumValueDecl,     // `val_1` in `enum { val_1, val_2 }`
        externDecl,        // `extern func foo();`
        genericParamDecl,  // `T` in `type C[T] = ...`
        importDecl,        // `import "otherMod"`
        moduleDecl,        // `mod myMod;`
        typeAliasDecl,     // `alias int_t = i32;`
        typeDecl,          // `type C[T] = ...`
    };

    struct Decl {
        DeclType type;
    };

    // Statements
    enum class StmtType : std::uint8_t {
        aliasStmt,
        block,
        boolLit,
        breakStmt,
        caseStmt,
        charLit,
        continueStmt,
        declStmt,
        exprStmt,
        forEachStmt,
        forStmt,
        identifierLit,
        ifStmt,
        integerLit,
        letStmt,
        returnStmt,
        stringLit,
        switchStmt,
        typeStmt,
    };

    struct Stmt {
        std::variant<Decl, Expr> data;
        StmtType type;
    };

    using StmtPtr = std::unique_ptr<Stmt>;
}  // namespace Winter

#endif  // WINTER_AST_H
