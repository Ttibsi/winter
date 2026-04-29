#include <willow/reporters.h>
#include <willow/willow.h>

#include "lexer_test.h"

int main(int argc, char* argv[]) {
    Willow::PreCommitReporter reporter = {};

    Willow::registerTests({
        // lexer_test.h
        {"between", test_between},
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
    });

    if (argc > 1) { return Willow::runSingleTest(std::string(argv[1]), reporter); }

    return Willow::runTests(reporter);
}
