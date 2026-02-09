#include <print>
#include <span>
#include <string>
#include <string_view>
#include <vector>

#include "compiler.h"

using namespace std::literals::string_view_literals;

constexpr auto usage() -> int {
    const std::string usage =
        "Usage:\n"
        "    winter [options] [file...]";

    std::println("{}", usage);
    return 1;
}

constexpr auto default_output() -> int {
    std::println("Winter: \x1b[31m\x1b[1mError: \x1b[0mNo input files");
    std::println("Compilation terminated");
    return 1;
}

auto main(int argc, char* argv[]) -> int {
    auto args = std::vector<std::string_view>(
        std::from_range, std::span {argv, static_cast<std::size_t>(argc)});

    bool enable_debug = false;
    std::string file = "";

    // TODO: -o flag for binary name
    if (args.size() == 1) { return default_output(); }
    for (auto&& arg : args) {
        if (arg == "-D"sv) { enable_debug = true; }
        if (arg.ends_with(".wtx"sv)) { file = arg; }
        if (arg == "--help"sv) { return usage(); }
    }

    if (file.empty()) { return default_output(); }

    auto compiler =
        Winter::Compiler(Winter::getBinaryName(file), Winter::getSourceCode(file), enable_debug);

    return 0;
}
