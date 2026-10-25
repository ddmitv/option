
#include <tests.hpp>
#include <opt/option.hpp>

namespace {
struct X {
    [[maybe_unused]] X() = default;
    constexpr int test() const& { return 3; }
    constexpr int test() & { return 4; }
    constexpr int test() const&& { return 5; }
    constexpr int test() && { return 6; }
};

template <typename T>
constexpr void test_contract() {
    opt::option<T> o;

    static_assert(std::is_same_v<decltype(o.value()), T&>);
    static_assert(std::is_same_v<decltype(std::as_const(o).value()), const T&>);
    static_assert(std::is_same_v<decltype(std::move(o).value()), T&&>);
    static_assert(std::is_same_v<decltype(std::move(std::as_const(o)).value()), const T&&>);

    static_assert(!noexcept(o.value()));
    static_assert(!noexcept(std::as_const(o).value()));
    static_assert(!noexcept(std::move(o).value()));
    static_assert(!noexcept(std::move(std::as_const(o)).value()));
}

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
        opt::option<X> opt(X{});

        check(std::as_const(opt).value().test() == 3);
        check(opt.value().test() == 4);
        check(std::move(std::as_const(opt)).value().test() == 5);
        check(std::move(opt).value().test() == 6);
    }
    return true;
}

}

void observe_methods_test_suite() {
    static_assert(test());
    test();

    OPTION_CLANG_SUPPRESS_WARNING_PUSH
    OPTION_CLANG_SUPPRESS_WARNING("-Wconsumed")
    {
        opt::option<X> o;
        check_throw((void)o.value(), opt::bad_access);
        check_throw((void)std::as_const(o).value(), opt::bad_access);
        check_throw((void)std::move(o).value(), opt::bad_access);
        check_throw((void)std::move(std::as_const(o)).value(), opt::bad_access);
    }
    {
        opt::option<X> o(X{});
        check_no_throw((void)o.value());
        check_no_throw((void)std::as_const(o).value());
        check_no_throw((void)std::move(o).value());
        check_no_throw((void)std::move(std::as_const(o)).value());
    }
    OPTION_CLANG_SUPPRESS_WARNING_POP
}
