#include <willow/reporters.h>
#include <willow/willow.h>

#include "lexer_test.h"
#include "parser_test.h"

int main(int argc, char* argv[]) {
    Willow::PreCommitReporter reporter = {};

    Willow::registerTests({
        // lexer_test.h
        {"between", test_between},
        {"token_toString", test_token_toString},
        {"token_toNum", test_token_toNum},
        {"skipWhitespace", test_skipWhitespace},
        {"skipComment", test_skipComment},
        {"isNumeric", test_isNumeric},
        {"isLetter", test_isLetter},
        {"lexSingle", test_lexSingle},
        {"lexDouble", test_lexDouble},
        {"lexChar", test_lexChar},
        {"lexString", test_lexString},
        {"lexNumeric", test_lexNumeric},
        {"lexIdentKeyword", test_lexIdentKeyword},
        {"operator()", test_operator_funcCall},

        // parser_test.h
        {"parserCheck", test_parser_check},
        {"parserConsumeVoid", test_parser_consume_void},
        {"parserConsumeTokens", test_parser_consume_tokens},
        {"parserParseBody", test_parser_parseBody},
        {"parserParseExpr", test_parser_parseExpr},
        {"parserParseFunc", test_parser_parseFunc},
        {"parserParseLet", test_parser_parseLet},
        {"parserParseNumLit", test_parser_parseNumLit},
        {"parserParseParam", test_parser_parseParam},
        {"parserParseReturn", test_parser_parseReturn},
        {"parserOperatorCall", test_parser_operator_call},

    });

    if (argc > 1) { return Willow::runSingleTest(std::string(argv[1]), reporter); }

    return Willow::runTests(reporter);
}
