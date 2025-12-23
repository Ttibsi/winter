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

constexpr int test_cmpDouble([[maybe_unused]] Willow::Test* test) {
    if (!cmpDouble(3.0, 4.5)) {
        return 1;
    }
    if (!cmpDouble(3.0, 3.0001)) {
        return 2;
    }
    return 0;
}
