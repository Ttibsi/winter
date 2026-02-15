#include "compiler.h"

#include <filesystem>
#include <fstream>
#include <sstream>

#include "parser.h"

namespace Winter {

    [[nodiscard]] auto getBinaryName(std::string_view filepath) -> std::string {
        std::filesystem::path path(filepath);
        return path.stem().string();
    }

    [[nodiscard]] auto getSourceCode(std::string_view path) -> std::string {
        std::ifstream ifs(path.data());
        std::stringstream buf_stream;
        buf_stream << ifs.rdbuf();
        return buf_stream.str();
    }

    auto Compiler::go() -> void {
        Lexer L = Lexer(src, debug);
        Parser P = Parser(&L);
        auto mainMod = P.parse();
    }

}  // namespace Winter
