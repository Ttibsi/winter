#include <print>
#include <span>
#include <string>
#include <string_view>

#include "winter.h"

using namespace std::string_view_literals;

void usage() {}

int main(int argc, char* argv[]) {
    bool enable_debug;
    std::string filename = "";

    const auto args = std::span(argv, argc);
    if (args.size() == 1) {
        usage();
        return 1;
    }

    for (auto&& elem : args) {
        std::string_view str = std::string_view(elem);

        if (str == "-D"sv) { enable_debug = true; }
        if (str.ends_with(".wtx"sv)) { filename = std::string(elem); }
    }

    if (filename.empty()) {
        std::println("Error: No file provided");
        usage();
        return 1;
    }

    // TODO: here goes the lexing/parsing etc

    return 0;
}
