#include <opt/option.hpp>

#include <tests.hpp>
#include <type_traits>
#include <vector>

// template<typename U>
// option(const option<U>&)

namespace {

template<typename T, typename U>
constexpr void test_implicit(const opt::option<U>& rhs, bool is_going_to_throw = false) {
    bool rhs_engaged = rhs.has_value();
#if 1
    try {
        opt::option<T> lhs = rhs;
        check(is_going_to_throw == false);
        check(lhs.has_value() == rhs_engaged);
        if (rhs_engaged) {
            check(*lhs == *rhs);
        }
    } catch (int i) {
        check(i == 6);
    }
#else
    if (is_going_to_throw) {
        return;
    }
    opt::option<T> lhs = rhs;
    check(lhs.has_value() == rhs_engaged);
    if (rhs_engaged) {
        check(*lhs == *rhs);
    }
#endif
}

struct implicit_X {
    int i;

    constexpr implicit_X(int i_) : i(i_) {}
    constexpr implicit_X(const implicit_X& x) : i(x.i) {}
    constexpr ~implicit_X() { i = 0; }
    friend bool operator==(const implicit_X& x, const implicit_X& y) = default;
};

struct implicit_Y {
    int i;
    constexpr implicit_Y(int i_) : i(i_) {}
    friend bool operator==(const implicit_Y& x, const implicit_Y& y) = default;
};

struct implicit_Z {
    int i;
    [[noreturn]] implicit_Z(int i_) : i(i_) { throw 6; }
    friend bool operator==(const implicit_Z&, const implicit_Z&) = default;
};

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/const_optional_U.pass.cpp
template<typename T, typename U>
constexpr bool test_all_implicit() {
    {
        opt::option<U> rhs;
        test_implicit<T>(rhs);
    }
    {
        opt::option<U> rhs(U{3});
        test_implicit<T>(rhs);
    }
    return true;
}

// TODO: test_ref

template<typename T, typename U>
constexpr void test_explicit(const opt::option<U>& rhs, bool is_going_to_throw = false) {
    static_assert(!std::is_convertible_v<const opt::option<U>&, opt::option<T>>);
    bool rhs_engaged = rhs.has_value();
#if 1
    try {
        opt::option<T> lhs(rhs);
        check(is_going_to_throw == false);
        check(lhs.has_value() == rhs_engaged);
        if (rhs_engaged) {
            check(*lhs == T(*rhs));
        }
    } catch (int i) {
        check(i == 6);
    }
#else
    if (is_going_to_throw) {
        return;
    }
    opt::option<T> lhs(rhs);
    assert(lhs.has_value() == rhs_engaged);
    if (rhs_engaged) {
        assert(*lhs == T(*rhs));
    }
#endif
}

class explicit_X {
    int i_;
public:
    constexpr explicit explicit_X(int i) : i_(i) {}
    constexpr explicit_X(const explicit_X& x) : i_(x.i_) {}
    constexpr ~explicit_X() { i_ = 0; }
    friend constexpr bool operator==(const explicit_X& x, const explicit_X& y) { return x.i_ == y.i_; }
};
class explicit_Y {
    int i_;
public:
    constexpr explicit explicit_Y(int i) : i_(i) {}
    friend constexpr bool operator==(const explicit_Y& x, const explicit_Y& y) { return x.i_ == y.i_; }
};
class explicit_Z {
    int i_;
public:
    [[noreturn]] explicit explicit_Z(int i) : i_(i) { throw 6; }

    friend bool operator==(const explicit_Z& x, const explicit_Z& y) { return x.i_ == y.i_; }
};

template<typename T, typename U>
constexpr bool test_all_explicit() {
    {
        opt::option<U> rhs;
        test_explicit<T>(rhs);
    }
    {
        opt::option<U> rhs(3);
        test_explicit<T>(rhs);
    }
    return true;
}

}

void conv_copy_ctor_test_suite() {
    test_all_implicit<int, short>();
    test_all_implicit<implicit_X, int>();
    test_all_implicit<implicit_Y, int>();
    static_assert(test_all_implicit<int, short>());
    static_assert(test_all_implicit<implicit_X, int>());
    static_assert(test_all_implicit<implicit_Y, int>());

    {
        opt::option<int> rhs;
        test_implicit<implicit_Z>(rhs);
    }
    {
        opt::option<int> rhs{3};
        test_implicit<implicit_Z>(rhs, true);
    }
    static_assert(!std::is_constructible_v<opt::option<implicit_X>, const opt::option<implicit_Y>&>);
    // TODO: test_ref

    test_all_explicit<explicit_X, int>();
    test_all_explicit<explicit_Y, int>();
    static_assert(test_all_explicit<explicit_X, int>());
    static_assert(test_all_explicit<explicit_Y, int>());
    {
        opt::option<int> rhs;
        test_explicit<explicit_Z>(rhs);
    }
    {
        opt::option<int> rhs(3);
        test_explicit<explicit_Z>(rhs, true);
    }
}
