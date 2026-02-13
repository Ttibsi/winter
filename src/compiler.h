#include <string>
#include <string_view>

namespace Winter {
    struct Compiler {
        int status_code = 0;
        std::string bin_name;
        std::string src;
        bool debug;

        explicit constexpr Compiler(std::string binary, std::string source, bool debug_mode)
            : bin_name(binary), src(std::move(source)), debug(debug_mode) {}

        auto go() -> void;
    };

    [[nodiscard]] auto getBinaryName(std::string_view filepath) -> std::string;
    [[nodiscard]] auto getSourceCode(std::string_view path) -> std::string;

}  // namespace Winter
