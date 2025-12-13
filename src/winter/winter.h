#ifndef WINTER_H
#define WINTER_H

#include <expected>
#include <functional>
#include <print>
#include <stack>
#include <string>
#include <string_view>
#include <unordered_map>

#include "error.h"
#include "helpers.h"
#include "lexer.h"
#include "object.h"

namespace Winter {

    struct VM {
        using WinterFn = std::function<int(VM&)>;

        bool debug;
        std::stack<Object> stack;
        std::unordered_map<std::string, WinterFn> registeredFunctions = {};

        explicit VM(bool dbg_mode) : debug(dbg_mode) {}
        constexpr void push(Object obj) { stack.push(obj); }

        [[nodiscard]] constexpr std::expected<Object, Err> pop() {
            if (stack.empty()) {
                return std::unexpected(Err(Err::ErrType::RuntimeError, "Stack is empty"));
            }
            Object obj = stack.top();
            stack.pop();
            return obj;
        }

        constexpr void registerFunc(const std::string& name, WinterFn func) {
            registeredFunctions.insert({name, func});
        }

        [[nodiscard]] constexpr retcode_t doString(const std::string& code) {
            Lexer l = Lexer(code);
            std::expected<void, Err> ret = l.tokenize();
            if (debug) {
                std::println("GO: {}", l);
            }

            return 0;
        }

        [[nodiscard]] constexpr retcode_t doFile(std::string_view fileName) {
            const std::string fText = openFile(fileName);
            retcode_t retcode = doString(fText);
            if (!retcode.has_value()) {
                return retcode;
            }

            // Enforcing that any full script contains the main function
            retcode = call("main");
            return retcode;
        }

        [[nodiscard]] constexpr retcode_t call(const std::string& funcName) {
            if (registeredFunctions.contains(funcName)) {
                return registeredFunctions.at(funcName)(*this);
            }

            // TODO: use std::format to include funcName in error message
            return std::unexpected(Err(Err::ErrType::NameError, "Name is not defined"));
        }
    };
}  // namespace Winter

#endif  // WINTER_H
