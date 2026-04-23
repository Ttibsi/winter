#include <string_view>

#include <willow/willow.h>

#include "parser.h"

using namespace std::literals::string_view_literals;

auto test_parse([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parse());
    return 0;
}

auto test_next([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.next());
    return 0;
}

auto test_parse_mod([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseMod());
    return 0;
}

auto test_parse_alias([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseAlias());
    return 0;
}

auto test_parse_generics([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseGenerics());
    return 0;
}

auto test_parse_body([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseBody());
    return 0;
}

auto test_parse_if([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseIf());
    return 0;
}

auto test_parse_for([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseFor());
    return 0;
}

auto test_parse_for_each([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseForEach("x"));
    return 0;
}

auto test_parse_switch([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseSwitch());
    return 0;
}

auto test_parse_case_arm([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseCaseArm());
    return 0;
}

auto test_parse_default_arm([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseDefaultArm());
    return 0;
}

auto test_parse_return([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseReturn());
    return 0;
}

auto test_parse_break([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseBreak());
    return 0;
}

auto test_parse_continue([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseContinue());
    return 0;
}

auto test_parse_expression([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseExpression());
    return 0;
}

auto test_parse_const([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseConst());
    return 0;
}

auto test_parse_function([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseFunction());
    return 0;
}

auto test_parse_attribute_method([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseAttributeMethod());
    return 0;
}

auto test_parse_class([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseClass());
    return 0;
}

auto test_parse_enum([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseEnum());
    return 0;
}

auto test_parse_interface([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseInterface());
    return 0;
}

auto test_parse_interface_attribute([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    Winter::InterfaceDef iface;
    static_cast<void>(p.parseInterfaceAttribute(iface, ""));
    return 0;
}

auto test_parse_interface_method([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    Winter::InterfaceDef iface;
    static_cast<void>(p.parseInterfaceMethod(iface, ""));
    return 0;
}

auto test_parse_type_definition([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseTypeDefinition());
    return 0;
}

auto test_parse_let([[maybe_unused]] Willow::Test* test) -> int {
    Winter::Parser p;
    p.L.src = "";
    static_cast<void>(p.parseLet());
    return 0;
}
