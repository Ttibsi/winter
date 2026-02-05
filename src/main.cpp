#include <print>
#include <span>
#include <string>
#include <string_view>
#include <vector>

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
    auto args = std::vector<std::string_view>(std::from_range, std::span {argv, argc});

    bool enable_debug = false;
    std::string file = "";

    if (args.size() == 1) { return default_output(); }
    for (auto&& arg : args) {
        if (arg == "-D"sv) { enable_debug = true; }
        if (arg.ends_with(".wtx"sv)) { file = arg; }
        if (arg == "--help"sv) { return usage(); }
    }

    if (file.empty()) { return default_output(); }

    return 0;
}
