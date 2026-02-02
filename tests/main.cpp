#include <willow/reporters.h>
#include <willow/willow.h>

int main() {
    Willow::PreCommitReporter reporter = {};

    return Willow::runTests(
        {

        },
        reporter);
}
