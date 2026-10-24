
#include <opt/option.hpp>

#include <ctors/test_types.hpp>
#include <tests.hpp>
#include <type_traits>
#include <vector>

namespace {

struct A {};

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/deduct.pass.cpp
constexpr bool test() {
    { // option(T)
        opt::option o(5);
        static_assert(std::is_same_v<decltype(o), opt::option<int>>);
        check(o.has_value());
        check(o.get() == 5);
    }
    { // option(T)
        opt::option o(A{});
        static_assert(std::is_same_v<decltype(o), opt::option<A>>);
        check(o.has_value());
    }
    { // option(const T&)
        const int& x = 5;
        opt::option o(x);
        static_assert(std::is_same_v<decltype(o), opt::option<int>>);
        check(o.has_value());
        check(o.get() == 5);
    }
    { // option(T*)
        const int* x = nullptr;
        opt::option o(x);
        static_assert(std::is_same_v<decltype(o), opt::option<const int*>>);
        check(o.has_value());
        check(o.get() == nullptr);
    }
    { // option(T[])
        int x[] = {1, 2, 3};
        opt::option o(x);
        static_assert(std::is_same_v<decltype(o), opt::option<int*>>);
        check(o.has_value());
        check(o.get()[0] == 1);
    }
    { // option(option), test implicit deduction guides
        opt::option<char> o1('A');
        opt::option o2(o1);
        static_assert(std::is_same_v<decltype(o2), opt::option<char>>);
        check(o2.has_value() == o1.has_value());
        check(o2.get() == o1.get());
    }
    return true;
}

}

void deduction_guides_test_suite() {
    test();
    static_assert(test());
}
