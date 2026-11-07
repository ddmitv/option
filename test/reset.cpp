#include <opt/option.hpp>
#include <tests.hpp>

// void reset() noexcept;

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.mod/reset.pass.cpp

namespace {

struct X {
    static inline bool dtor_called = false;

    [[maybe_unused]] X() = default;
    X(const X&) = default;
    [[maybe_unused]] X& operator=(const X&) = default;
    ~X() { dtor_called = true; }
};

constexpr bool check_reset() {
    {
        opt::option<int> opt;
        static_assert(noexcept(opt.reset()));
        opt.reset();
        check(!opt.has_value());
    }
    {
        opt::option<int> opt(3);
        opt.reset();
        check(!opt.has_value());
    }
    return true;
}

}

void reset_test_suite() {
    check_reset();
    static_assert(check_reset());

    {
        opt::option<X> opt;
        static_assert(noexcept(opt.reset()));
        check(X::dtor_called == false);
        opt.reset();
        check(X::dtor_called == false);
        check(!opt.has_value());
    }
    {
        opt::option<X> opt(X{});
        X::dtor_called = false;
        opt.reset();
        check(X::dtor_called == true);
        check(!opt.has_value());
        X::dtor_called = false;
    }

    // TODO: test_ref
}
