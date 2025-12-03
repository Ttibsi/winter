#include "winter/winter.h"

int main() {
    Winter::VM vm = {};

    vm.push(Winter::Object(Winter::ObjType::Null, 5));
    vm.push(Winter::Object(Winter::ObjType::Null, 3));

    vm.registerFunc("add", [](Winter::VM& vm) {
        auto b = vm.pop().unwrap<int>();
        auto a = vm.pop().unwrap<int>();
        vm.push(Winter::Object(Winter::ObjType::Null, a + b);
        return 0;
    });

    return 0;
}
