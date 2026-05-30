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
        {"token_toChar", test_token_toChar},
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
        {"nodeOperatorEQ", test_node_op_eq},
        {"parserCheck", test_parser_check},
        {"parserConsumeVoid", test_parser_consume_void},
        {"parserConsumeTokens", test_parser_consume_tokens},
        {"parserParseAlias", test_parser_parseAlias},
        {"parserParseArg", test_parser_parseArg},
        {"parserParseBody", test_parser_parseBody},
        {"parserParseCallOrVariable", test_parser_parseCallOrVariable},
        {"parserParseCase", test_parser_parseCase},
        {"parserParseCharLit", test_parser_parseCharLit},
        {"parserParseClass", test_parser_parseClass},
        {"parserParseEnum", test_parser_parseEnum},
        {"parserParseExpr", test_parser_parseExpr},
        {"parserParseFor", test_parser_parseFor},
        {"parserParseForEach", test_parser_parseForEach},
        {"parserParseFunc", test_parser_parseFunc},
        {"parserParseFuncCall", test_parser_parseFuncCall},
        {"parserParseIf", test_parser_parseIf},
        {"parserParseMod", test_parser_parseMod},
        {"parserParseLet", test_parser_parseLet},
        {"parserParseNumLit", test_parser_parseNumLit},
        {"parserParseParam", test_parser_parseParam},
        {"parserParseReturn", test_parser_parseReturn},
        {"parserParseSwitch", test_parser_parseSwitch},
        {"parserParseStrLit", test_parser_parseStrLit},
        {"parserParseType", test_parser_parseType},
        {"parserParseVariable", test_parser_parseVariable},
        {"parserOperatorCall", test_parser_operatorCall},

    });

    if (argc > 1) { return Willow::runSingleTest(std::string(argv[1]), reporter); }

    return Willow::runTests(reporter);
}
