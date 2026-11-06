#include <opt/option.hpp>

#include <tests.hpp>
#include <type_traits>
#include <vector>

// template<typename U>
// option(option<U>&&)

namespace {

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/optional_U.pass.cpp


template <typename T, typename U>
constexpr void test_implicit(opt::option<U>&& rhs, bool is_going_to_throw = false) {
    bool rhs_engaged = rhs.has_value();
#if 1
    try {
        opt::option<T> lhs = std::move(rhs);
        check(is_going_to_throw == false);
        check(lhs.has_value() == rhs_engaged);
    } catch (int i) {
        check(i == 6);
    }
#else
    if (is_going_to_throw) {
        return;
    }
    opt::option<T> lhs = std::move(rhs);
    check(lhs.has_value() == rhs_engaged);
#endif
}

class implicit_X {
    int i_;
public:
    constexpr implicit_X(int i) : i_(i) {}
    [[maybe_unused]] constexpr implicit_X(implicit_X&& x) : i_(std::exchange(x.i_, 0)) {}
    constexpr ~implicit_X() { i_ = 0; }
    [[maybe_unused]] friend constexpr bool operator==(const implicit_X& x, const implicit_X& y) { return x.i_ == y.i_; }
};

struct implicit_Z {
    [[noreturn]] implicit_Z(int) { throw 6; }
};

template<typename T, typename U>
constexpr bool test_all_implicit() {
    {
        opt::option<T> rhs;
        test_implicit<U>(std::move(rhs));
    }
    {
        opt::option<T> rhs(short{3});
        test_implicit<U>(std::move(rhs));
    }
    return true;
}

// TODO: test_ref

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/explicit_optional_U.pass.cpp
template<typename T, typename U>
constexpr void test_explicit(opt::option<U>&& rhs, bool is_going_to_throw = false) {
    static_assert(!std::is_convertible_v<opt::option<U>&&, opt::option<T>>);
    bool rhs_engaged = rhs.has_value();
#if 1
    try {
        opt::option<T> lhs(std::move(rhs));
        check(is_going_to_throw == false);
        check(lhs.has_value() == rhs_engaged);
    } catch (int i) {
        check(i == 6);
    }
#else
    if (is_going_to_throw) {
        return;
    }
    opt::option<T> lhs(std::move(rhs));
    check(lhs.has_value() == rhs_engaged);
#endif
}

class explicit_X {
    int i_;
public:
    constexpr explicit explicit_X(int i) : i_(i) {}
    [[maybe_unused]] constexpr explicit_X(explicit_X&& x) : i_(x.i_) { x.i_ = 0; }
    constexpr ~explicit_X() { i_ = 0; }
    [[maybe_unused]] friend constexpr bool operator==(const explicit_X& x, const explicit_X& y) { return x.i_ == y.i_; }
};

class explicit_Z {
public:
    [[noreturn]] explicit explicit_Z(int) { throw 6; }
};

constexpr bool test_all_explicit() {
    {
        opt::option<int> rhs;
        test_explicit<explicit_X>(std::move(rhs));
    }
    {
        opt::option<int> rhs(3);
        test_explicit<explicit_X>(std::move(rhs));
    }
    return true;
}

}

void conv_move_ctor_test_suite() {
    test_all_implicit<short, int>();
    test_all_implicit<int, implicit_X>();
    static_assert(test_all_implicit<short, int>());
    static_assert(test_all_implicit<int, implicit_X>());
    {
        opt::option<int> rhs;
        test_implicit<implicit_Z>(std::move(rhs));
    }
#if !GCC_HWASAN_UNWINDER_BROKEN
    {
        opt::option<int> rhs(3);
        test_implicit<implicit_Z>(std::move(rhs), true);
    }
#endif
    static_assert(!std::is_constructible_v<opt::option<implicit_X>, opt::option<implicit_Z>>);

    // TODO: test_ref

    static_assert(test_all_explicit());
    test_all_explicit();
    {
        opt::option<int> rhs;
        test_explicit<explicit_Z>(std::move(rhs));
    }
#if !GCC_HWASAN_UNWINDER_BROKEN
    {
        opt::option<int> rhs(3);
        test_explicit<explicit_Z>(std::move(rhs), true);
    }
#endif
}
