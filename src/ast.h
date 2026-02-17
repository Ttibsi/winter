#ifndef WINTER_AST_H
#define WINTER_AST_H

#include <optional>
#include <string>
#include <variant>
#include <vector>

#include "lexer.h"

namespace Winter {

    // pre-declarations
    struct aliasNode;
    struct argNode;
    struct assignNode;
    struct binaryExpr;
    struct blockNode;
    struct classNode;
    struct enumNode;
    struct forEachStmt;
    struct forStmt;
    struct funcCallStmt;
    struct funcNode;
    struct ifStmt;
    struct includeNode;
    struct interfaceNode;
    struct letStmt;
    struct methodCallStmt;
    struct moduleNode;
    struct retStmt;
    struct tertiaryExpr;
    struct typeDeclarationNode;
    struct typeNode;
    struct unaryExpr;
    struct varDeclarationNode;

    // variants
    using Stmt_t = std::variant<forStmt*, ifStmt*, retStmt*, funcCallStmt*, methodCallStmt*>;
    using Expr_t = std::variant<binaryExpr, tertiaryExpr, unaryExpr>;
    using ExprStmt_t = std::variant<Expr_t, Stmt_t>;
    using Definitions_t = std::variant<letStmt, typeNode>;
    using Declarations_t = std::variant<varDeclarationNode*, typeDeclarationNode*>;
    using BlockItem_t =
        std::variant<Stmt_t*, letStmt*, assignNode*, Expr_t*, typeNode*, aliasNode*>;

    // definitions

    // ALIAS IDENT EQUAL IDENT SEMICOLON
    // ALIAS IDENT EQUAL FUNC...
    struct aliasNode {
        std::string_view name;
        std::string_view type;
    };

    // IDENT COLON IDENT
    // name        type
    struct argNode {
        std::string name = "";
        std::string type = "";
    };

    // IDENT EQUAL expr
    struct assignNode {
        std::string lhs = "";
        Expr_t rhs;
    };

    struct binaryExpr {
        Expr_t lhs;
        TokenType symbol;
        Expr_t rhs;
    };

    struct blockNode {
        std::vector<BlockItem_t> items = {};
    };

    struct classNode {
        std::optional<std::string> interfaceName;
        std::vector<letStmt*> attributes = {};
        std::vector<letStmt*> methods = {};
    };

    // ENUM LBRACE (IDENT COMMA) RBRACE
    struct enumNode {
        std::string enumNamespace;
        std::vector<std::string> idents = {};
    };

    struct forStmt {
        letStmt* start;
        assignNode* stop;
        unaryExpr* step;
        blockNode* body;
    };

    // FOR LPAREN IDENT COLON IDENT RPAREN BLOCK
    struct forEachStmt {
        std::string element;
        std::string container;
    };

    // IDENT LPAREN (IDENT|EXPR) SEMICOLON
    struct funcCallStmt {
        std::string ident;
        std::vector<Expr_t> params;
    };

    // FUNC LPAREN (argNode...) RPAREN IDENT LBRACE blockNode RBRACE
    struct funcNode {
        std::vector<argNode> arguments = {};
        std::string returnType;
        blockNode block;
    };

    // IF LPAREN Expr_t RPAREN blockNode ELSE [block|IF]
    struct ifStmt {
        Expr_t expr;
        blockNode* ifBlock;
        std::optional<blockNode*> elseBlock;
    };

    // IMPORT IDENT
    struct includeNode {
        std::string modName;
    };

    struct interfaceNode {
        std::vector<Declarations_t> declarations = {};
    };

    // LET IDENT (COLON IDENT) EQUAL Expr_t SEMICOLON
    // LET IDENT (LSQUACKET (IDENT COMMA) RSQUACKET) EQUAL [FUNC|IMPORT|CLASS|INTERFACE]
    struct letStmt {
        std::string name = "";
        std::vector<std::string> generics = {};
        std::optional<std::string> typeLiteral;
        std::variant<funcNode, includeNode> body;
    };

    struct methodCallStmt {
        std::string objectName;
        funcCallStmt* methodCall;
    };

    // MOD, IDENT, SEMICOLON, [let|type {} ]
    struct moduleNode {
        std::string moduleName = "";
        std::vector<aliasNode> aliases = {};
        std::vector<Definitions_t> contents = {};
    };

    struct retStmt {
        Expr_t expr;
    };

    // NOTE: Only tertiary expr is ?:
    struct tertiaryExpr {
        Expr_t left;
        Expr_t middle;
        Expr_t right;
    };

    // TYPE IDENT EQUAL [enum|class|interface]
    struct typeNode {
        std::string name = "";
        std::vector<std::string> generics = {};
        std::optional<std::string> typeLiteral;
        std::variant<enumNode, classNode, interfaceNode> body;
    };

    // TYPE IDENT EQUAL ...
    struct typeDeclarationNode {
        std::string name;
        TokenType type;  // func, enum, class

        // func
        std::optional<std::vector<argNode*>> args;
        std::optional<std::string> returnType;

        // class implements interface
        std::optional<std::string> interfaceName;
    };

    struct unaryExpr {
        TokenType symbol;
        Expr_t operand;
    };

    // LET IDENT COLON IDENT SEMI
    struct varDeclarationNode {
        std::string name;
        std::optional<std::string> type;
    };

}  // namespace Winter

#endif  // WINTER_AST_H
