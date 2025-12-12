#include <string>

#include "winter/helpers.h"

constexpr int test_openFile([[maybe_unused]] Willow::Test* test) {
    const std::string f = openFile("tests/fixture/test_file_1.txt");

    // wc -c tests/fixture/test_file_1.txt
    if (f.size() != 70) {
        return 1;
    }
    if (f.at(0) != 'T') {
        return 2;
    }

    return 0;
}
