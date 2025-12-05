#include <willow/reporters.h>
#include <willow/willow.h>

#include "helpers_test.h"
#include "object_test.h"
#include "winter_test.h"

int main() {
    Willow::PreCommitReporter reporter = {};

    return Willow::runTests(
        {
            {"openFile", test_openFile},
            {"[Object] Constructor", test_objectConstructor},
            {"[Object] unwrap", test_objectUnwrap},
            {"[Err] Constructor", test_errConstructor},
            {"[VM] push", test_vmPush},
            {"[VM] pop", test_vmPop},
            {"[VM] doString", test_vmDoString, Willow::Status::Skip},
            {"[VM] doFile", test_vmDoFile, Willow::Status::Skip},
            {"[VM] call", test_vmCall, Willow::Status::Skip},
        },
        reporter);
}
