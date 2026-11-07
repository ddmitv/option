
#include <opt/option.hpp>
#include <tests.hpp>

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.dtor/dtor.pass.cpp

namespace {

struct pod_type {
    int value;
    int value2;
};

struct X {
    static inline bool dtor_called = false;
    X() = default;
    X(const X&) = default;
    X& operator=(const X&) = default;
    ~X() { dtor_called = true; }
};

}

void dtor_test_suite() {
    static_assert(std::is_trivially_destructible_v<int>);
    static_assert(std::is_trivially_destructible_v<opt::option<int>>);

    static_assert(std::is_trivially_destructible_v<double>);
    static_assert(std::is_trivially_destructible_v<opt::option<double>>);

    static_assert(std::is_trivially_destructible_v<pod_type>);
    static_assert(std::is_trivially_destructible_v<opt::option<pod_type>>);

    static_assert(!std::is_trivially_destructible_v<X>);
    static_assert(!std::is_trivially_destructible_v<opt::option<X>>);
    {
        X x;
        opt::option<X> opt{x};
        check(X::dtor_called == false);
    }
    check(X::dtor_called == true);

    // TODO: test_ref
}
