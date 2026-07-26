#include <fstream>
#include <memory>
#include <optional>
#include <print>
#include <span>
#include <sstream>
#include <string>
#include <string_view>
#include <vector>

#include <llvm/IR/Module.h>

#include "backend/backend.h"
#include "error.h"
#include "frontend/lexer.h"
#include "frontend/parser.h"

using namespace std::literals::string_view_literals;

[[nodiscard]] constexpr int usage() noexcept {
    const std::string usage =
        "Usage:\n"
        "    winter [options] [file...]\n"
        "\n"
        "   Options:\n"
        "   -D    enable debug mode and print debug info at each stage\n"
        "";

    std::println("{}", usage);
    return 1;
}

[[nodiscard]] constexpr int default_output() noexcept {
    std::println("Winter: \x1b[31m\x1b[1mError: \x1b[0mNo input files");
    std::println("Compilation terminated");
    return 1;
}

[[nodiscard]] std::string getSourceCode(std::string_view path) {
    std::ifstream ifs(path.data());
    std::stringstream buf_stream;
    buf_stream << ifs.rdbuf();
    return buf_stream.str();
}

[[nodiscard]] int compile(std::string_view file_name, bool dbg) noexcept {
    std::string src = getSourceCode(file_name);

    // Lexer
    Winter::Lexer L = Winter::Lexer(src);
    Winter::Token t = Winter::Token::tombstone();
    if (dbg) {
        std::println("=== LEXER ===");
        while (t.type != Winter::TokenType::eof) {
            auto ret = L();
            if (!ret.has_value()) {
                std::println("ERROR: {}", ret.error().msg);
                return -1;
            }

            t = ret.value();
            std::println("{}", ret.value());
        }

        std::println();
        Winter::Token::counter = 0;
    }

    // Parser
    Winter::Parser P = Winter::Parser(src);
    std::expected<std::vector<Winter::Node>, Winter::Error> result = P();
    if (!result.has_value()) {
        std::println("ERROR: {}", result.error().msg);
        return -1;
    }

    if (dbg) { P.display_syntax_tree(result.value()); }

    // backend
    Winter::Backend B = Winter::Backend(file_name);
    Winter::module_result_t backendRet = B.compileModule(result.value());
    if (!backendRet.has_value()) {
        std::println("ERROR: {}", backendRet.error().msg);
        return -1;
    }

    if (dbg) { B.display_module(backendRet.value()); }
    std::expected<std::string, Winter::Error> obj_err = B.outputObjectFile(backendRet.value());
    if (!obj_err.has_value()) {
        std::println("ERROR: {}", obj_err.error().msg);
        return -1;
    }

    std::vector<const char*> files = {obj_err.value().data()};
    std::optional<Winter::Error> linkErr = B.linkModules(files);
    if (linkErr.has_value()) {
        std::println("ERROR: {}", linkErr.value().msg);
        return -1;
    }

    return 0;
}

int main(int argc, char* argv[]) {
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
    return compile(file, enable_debug);
}
