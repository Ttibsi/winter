#include <print>
#include <span>
#include <string>
#include <string_view>

#include "winter/winter.h"

using namespace std::string_view_literals;

void startRepl() {}

int main(int argc, char* argv[]) {
    const auto args = std::span(argv, argc);

    if (args.size() == 1) {
        startRepl();
        return 0;
    }

    bool enable_debug;
    std::string filename = "";

    for (auto&& elem : args) {
        std::string_view str = std::string_view(elem);

        if (str == "-D"sv) {
            enable_debug = true;
        }
        if (str.ends_with(".wt"sv)) {
            filename = std::string(elem);
        }
    }

    if (filename.empty()) {
        std::println("Error: No file provided");
        return 1;
    }

    auto vm = Winter::VM(enable_debug);
    Winter::retcode_t ret = vm.doFile(filename);
    if (!ret.has_value()) {
        std::println("ERROR: {}", ret.error().msg);
    };

    return ret.value();
}
