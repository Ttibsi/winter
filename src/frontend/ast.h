#ifndef WINTER_AST_H
#define WINTER_AST_H

namespace Winter {
    // Expressions
    enum class ExprType : std::uint8_t {
        func_call
    };

    struct Expr {
        std::variant<> data;
        ExprType type;
    };

    using ExprPtr = std::unique_ptr<Expr>;

    // Declarations
    enum class DeclType : std::uint8_t {
        includeDecl,
        interfaceDecl,
        classDecl,
        enumDecl,
        funcDecl,
        varDecl,
    };

    struct Decl {
        DeclType type;
    };

    // Statements
    enum class StmtType : std::uint8_t {
        moduleStmt,
        aliasStmt,
        letStmt,
        typeStmt,
    };

    struct Stmt {
        std::variant<Decl, Expr> data;
        StmtType type;
    };

    using StmtPtr = std::unique_ptr<Stmt>;
}  // namespace Winter

#endif  // WINTER_AST_H
