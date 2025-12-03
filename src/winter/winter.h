#ifndef WINTER_H
#define WINTER_H

#include <stack>

#include "object.h"

namespace Winter {
    struct VM {
        using WinterFn = std::function<int(VM&)>;

        std::stack<Object> stack;

        constexpr void push(Object);
        [[nodiscard]] constexpr Object pop();
    };
}  // namespace Winter

#endif  // WINTER_H
