
#include <opt/option.hpp>
#include <tests.hpp>

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.observe/has_value.pass.cpp

void has_value_test_suite() {
    {
        const opt::option<int> o;
        check_only_meta(o.has_value(), bool, noexcept);
    }
    {
        constexpr opt::option<int> o;
        static_assert(!o.has_value());
    }
    {
        constexpr opt::option<int> o(0);
        static_assert(o.has_value());
    }
    // TODO: test_ref
}
