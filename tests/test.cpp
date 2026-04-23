#include <willow/reporters.h>
#include <willow/willow.h>

#include "lexer_test.h"
#include "parser_test.h"

auto main(int argc, char* argv[]) -> int {
    Willow::PreCommitReporter reporter = {};

    Willow::registerTests({
        // lexer_test.h
        {"skipWhitespace", test_skipWhitespace},
        {"skipComment", test_skipComment},
        {"between", test_between},
        {"isNumeric", test_isNumeric},
        {"isLetter", test_isLetter},
        {"lexNumeric", test_lexNumeric},
        {"lexSingle", test_lexSingle},
        {"lexDouble", test_lexDouble},
        {"lexChar", test_lexChar},
        {"lexString", test_lexString},
        {"lexIdentKeyword", test_lexIdentKeyword},
        {"operator()", test_operator_funcCall},
        // parser_test.h
        {"parse", test_parse},
        {"next", test_next},
        {"parseMod", test_parse_mod},
        {"parseAlias", test_parse_alias},
        {"parseGenerics", test_parse_generics},
        {"parseBody", test_parse_body},
        {"parseIf", test_parse_if},
        {"parseFor", test_parse_for},
        {"parseForEach", test_parse_for_each},
        {"parseSwitch", test_parse_switch},
        {"parseCaseArm", test_parse_case_arm},
        {"parseDefaultArm", test_parse_default_arm},
        {"parseReturn", test_parse_return},
        {"parseBreak", test_parse_break},
        {"parseContinue", test_parse_continue},
        {"parseExpression", test_parse_expression},
        {"parseConst", test_parse_const},
        {"parseFunction", test_parse_function},
        {"parseAttributeMethod", test_parse_attribute_method},
        {"parseClass", test_parse_class},
        {"parseEnum", test_parse_enum},
        {"parseInterface", test_parse_interface},
        {"parseInterfaceAttribute", test_parse_interface_attribute},
        {"parseInterfaceMethod", test_parse_interface_method},
        {"parseTypeDefinition", test_parse_type_definition},
        {"parseLet", test_parse_let},
    });

    if (argc > 1) { return Willow::runSingleTest(std::string(argv[1]), reporter); }

    return Willow::runTests(reporter);
}
