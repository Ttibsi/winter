#include <willow/reporters.h>
#include <willow/willow.h>

int main() {
    Willow::PreCommitReporter reporter = {};

    Willow::registerTests({});

    return Willow::runTests(reporter);
}
