#include <willow/reporters.h>
#include <willow/willow.h>

#include "bytecode_test.h"
#include "error_test.h"
#include "helpers_test.h"
#include "lexer_test.h"
#include "object_test.h"
#include "parser_test.h"
#include "winter_test.h"

int main() {
    Willow::PreCommitReporter reporter = {};

    return Willow::runTests(
        {
            {"openFile", test_openFile},
            {"[Object] Constructor", test_objectConstructor},
            {"[Object] unwrap", test_objectUnwrap},
            {"[Err] Constructor", test_errConstructor},
            {"[Err] getErrType", test_getErrType},
            {"[Err] display", test_display},
            {"[VM] push", test_vmPush},
            {"[VM] pop", test_vmPop},
            {"[VM] doString", test_vmDoString, Willow::Status::Skip},
            {"[VM] doFile", test_vmDoFile, Willow::Status::Skip},
            {"[VM] call", test_vmCall, Willow::Status::Skip},
            {"[Token] toString", test_token_toString},
            {"[Lexer] makeToken", test_makeToken},
            {"[Lexer] scanNumber", test_scanNumber},
            {"[Lexer] scanStringLiteral", test_scanStringLiteral},
            {"[Lexer] scanChar", test_scanChar},
            {"[Lexer] validIdentChar", test_validIdentChar},
            {"[Lexer] scanKeyword", test_scanKeyword},
            {"[Lexer] scanIdentifier", test_scanIdentifier},
            {"[Lexer] scanEllipsis", test_scanEllipsis},
            {"[Lexer] tokenize", test_tokenize},
            {"[Lexer] advance (TokenType overload)", test_advanceTokOverload},
            {"[Lexer] advance", test_advance},
            {"[Lexer] currToken", test_currToken},
            {"[Lexer] check", test_check},
            {"[Lexer] checkNext", test_checkNext},
            {"[Lexer] atEnd", test_atEnd},
            {"[Parser] prefixBindingPower", test_prefixBindingPower},
            {"[Parser] infixBindingPower", test_infixBindingPower},
            {"[Parser] parseBlock", test_parseBlock},
            {"[Parser] parseExpression", test_parseExpression},
            {"[Parser] parseFunc", test_parseFunc},
            {"[Parser] parseReturn", test_parseReturn},
            {"[Parser] parseStatement", test_parseStatement},
            {"[Parser] parse_tree", test_parse_tree},
            {"[Chunk] extend", test_Chunk_extend},
            {"[Generator] compileValue", test_Generator_compileValue},
            {"[Generator] compileTok", test_Generator_compileTok},
            {"[Generator] compileExpression", test_Generator_compileExpression},
            {"[Generator] compileReturn", test_Generator_compileReturn},
            {"[Generator] compileFunc", test_Generator_compileFunc},
            {"[Generator] generate", test_Generator_generate},
        },
        reporter);
}
