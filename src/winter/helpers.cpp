#include "helpers.h"

#include <fstream>
#include <sstream>

// https://sqlpey.com/c++/cpp-file-to-string-methods/
[[nodiscard]] std::string openFile(std::string_view fileName) {
    std::ifstream ifs(fileName.data());
    std::stringstream buf_stream;
    buf_stream << ifs.rdbuf();
    return buf_stream.str();
}
