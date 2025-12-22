#ifndef WINTER_HELPERS_H
#define WINTER_HELPERS_H

#include <string>
#include <string_view>

[[nodiscard]] std::string openFile(std::string_view);
[[nodiscard]] constexpr bool cmpDouble(const double&, const double&);

#endif  // WINTER_HELPERS_H
