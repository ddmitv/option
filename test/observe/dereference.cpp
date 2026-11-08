
#include <tests.hpp>
#include <opt/option.hpp>

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.observe/dereference.pass.cpp

namespace {

struct X {
    constexpr int test() const& { return 3; }
    constexpr int test() & { return 4; }
    constexpr int test() const&& { return 5; }
    constexpr int test() && { return 6; }
};

template<typename T>
constexpr void test_contract() {
    opt::option<T> o(T{});

    check_only_meta(*o, T&, noexcept);
    check_only_meta(*std::move(o), T&&, noexcept);
    check_only_meta(*std::as_const(o), const T&, noexcept);
    check_only_meta(*std::move(std::as_const(o)), const T&&, noexcept);
}

// TODO: test_ref

constexpr bool test() {
    test_contract<int>();
    test_contract<float>();
    test_contract<double>();
    test_contract<X>();
    test_contract<const int>();
    test_contract<const float>();
    test_contract<const double>();
    test_contract<const X>();

    {
        opt::option<X> o{X{}};
        check((*std::as_const(o)).test() == 3);
        check((*o).test() == 4);
        check((*std::move(std::as_const(o))).test() == 5);
        check((*std::move(o)).test() == 6);

        // Test that operator* returns a stable reference
        SUPPRESS_WCONSUMED(
        X& x = *o;
        check(&x == &(*o));
        )

        const X& x2 = *std::as_const(o);
        check(&x2 == &(*o));
    }
    return true;
}

}

void dereference_test_suite() {
    test();
    static_assert(test());
}
