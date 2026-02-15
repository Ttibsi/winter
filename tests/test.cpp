#include <willow/reporters.h>
#include <willow/willow.h>

#include "lexer_test.h"
#include "parser_test.h"

auto main(int argc, char* argv[]) -> int {
    Willow::PreCommitReporter reporter = {};

    Willow::registerTests({
        // lexer_test.h
        {"getString", test_getString},
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
    });

    if (argc > 1) { return Willow::runSingleTest(std::string(argv[1]), reporter); }

    return Willow::runTests(reporter);
}
