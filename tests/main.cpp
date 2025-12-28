#include <willow/reporters.h>
#include <willow/willow.h>

#include "error_test.h"
#include "helpers_test.h"
#include "lexer_test.h"
#include "object_test.h"
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
        },
        reporter);
}
