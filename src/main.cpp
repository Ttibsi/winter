#include <cassert>
#include <expected>
#include <print>
#include <span>
#include <string_view>

#include "winter/winter.h"

using namespace std::string_view_literals;

int main(int argc, char* argv[]) {
    bool enable_debug = false;

    auto args = std::span(argv, argc);
    if (args.size() > 1) {
        if (args[1] == "-D"sv) {
            enable_debug = true;
        }
    }

    auto vm = Winter::VM(enable_debug);

    vm.push(Winter::Object(Winter::ObjType::Null, 5));
    vm.push(Winter::Object(Winter::ObjType::Null, 3));

    vm.registerFunc("add", [](Winter::VM& vm) {
        auto b = vm.pop()->unwrap<int>();
        auto a = vm.pop()->unwrap<int>();
        vm.push(Winter::Object(Winter::ObjType::Null, a + b));
        return 0;
    });

    Winter::retcode_t ret = vm.call("add");
    assert(ret.has_value());

    const int value = vm.pop()->unwrap<int>();
    assert(value == 8);

    std::println("Result: {}", value);
    return 0;
}
