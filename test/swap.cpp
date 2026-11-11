#include <tests.hpp>
#include <opt/option.hpp>

#include <type_traits>
#include <utility>
#include <archetypes.hpp>

// void swap(option& other)
//     noexcept(is_nothrow_move_constructible<T>::value &&
//              is_nothrow_swappable<T>::value)

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.swap/swap.pass.cpp
// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.specalg/swap.pass.cpp

namespace {

class X {
    int i;
public:
    static inline int dtor_called = 0;

    X(int i_) : i(i_) {}
    X(X&&) = default;
    X& operator=(X&&) = default;
    ~X() { ++dtor_called; }

    friend bool operator==(const X& x, const X& y) { return x.i == y.i; }
};

class Y {
    int i;
public:
    static inline int dtor_called = 0;

    Y(int i_) : i(i_) {}
    Y(Y&&) = default;
    ~Y() { ++dtor_called; }

    friend bool operator==(const Y& x, const Y& y) { return x.i == y.i; }
    friend void swap(Y& x, Y& y) { std::swap(x.i, y.i); }
};

class W {
    int i;
public:
    constexpr W(int i_) : i(i_) {}

    friend constexpr bool operator==(const W& x, const W& y) { return x.i == y.i; }
    friend constexpr void swap(W& x, W& y) noexcept { std::swap(x.i, y.i); }
};

class Z {
    int i;
public:
    Z(int i_) : i(i_) {}
    [[noreturn]] Z(Z&&) { do_throw(7); }

    friend constexpr bool operator==(const Z& x, const Z& y) { return x.i == y.i; }
    [[noreturn]] friend void swap(Z&, Z&) { do_throw(6); }
};

struct non_swappable {
    non_swappable(non_swappable const&) = delete;
};
void swap(non_swappable&, non_swappable&) = delete;

template<typename T>
constexpr bool check_swap() {
    {
        opt::option<T> opt1;
        opt::option<T> opt2;
        static_assert(noexcept(opt1.swap(opt2)));
        check(!opt1.has_value());
        check(!opt2.has_value());
        opt1.swap(opt2);
        check(!opt1.has_value());
        check(!opt2.has_value());
    }
    {
        opt::option<T> opt1(1);
        opt::option<T> opt2;
        static_assert(noexcept(opt1.swap(opt2)));
        check(opt1.has_value());
        check(*opt1 == 1);
        check(!opt2.has_value());
        opt1.swap(opt2);
        check(!opt1.has_value());
        check(opt2.has_value());
        check(*opt2 == 1);
    }
    {
        opt::option<T> opt1;
        opt::option<T> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)));
        check(!opt1.has_value());
        check(opt2.has_value());
        check(*opt2 == 2);
        opt1.swap(opt2);
        check(opt1.has_value());
        check(*opt1 == 2);
        check(!opt2.has_value());
    }
    {
        opt::option<T> opt1(1);
        opt::option<T> opt2(2);
        static_assert(noexcept(opt1.swap(opt2)));
        check(opt1.has_value());
        check(*opt1 == 1);
        check(opt2.has_value());
        check(*opt2 == 2);
        opt1.swap(opt2);
        check(opt1.has_value());
        check(*opt1 == 2);
        check(opt2.has_value());
        check(*opt2 == 1);
    }
    return true;
}

// TODO: test_ref

void test_swap_sfinae() {
    static_assert(std::is_swappable_v<opt::option<test_types::test_type>>);
    static_assert(std::is_swappable_v<opt::option<test_types::move_only>>);
    static_assert(std::is_swappable_v<opt::option<test_types::copyable>>);
    static_assert(!std::is_swappable_v<opt::option<test_types::no_ctors>>);
    static_assert(!std::is_swappable_v<opt::option<non_swappable>>);
    {
        // Even though CopyOnly has deleted move operations, those operations
        // cause optional<CopyOnly> to have implicitly deleted move operations
        // that decay into copies.
        using T = test_types::copy_only;
        T::reset();
        opt::option<T> L(101), R(42);
        T::reset_constructors();
        std::swap(L, R);
        check(L->value == 42);
        check(R->value == 101);
        check(T::copy_constructed == 1);
        check(T::constructed == T::copy_constructed);
        check(T::assigned == 2);
        check(T::assigned == T::copy_assigned);
    }
}

// TODO: test_swap_ref

}

void swap_test_suite() {
    check_swap<int>();
    check_swap<W>();
    static_assert(check_swap<int>());
    static_assert(check_swap<W>());

    test_swap_sfinae();
    // TODO: test_swap_ref

    {
        opt::option<X> o1;
        opt::option<X> o2;
        static_assert(noexcept(o1.swap(o2)));
        check(!o1.has_value());
        check(!o2.has_value());
        o1.swap(o2);
        check(!o1.has_value());
        check(!o2.has_value());
        check(X::dtor_called == 0);
    }
    {
        opt::option<X> o1(1);
        opt::option<X> o2;
        static_assert(noexcept(o1.swap(o2)));
        check(o1.has_value());
        check(*o1 == 1);
        check(!o2.has_value());
        X::dtor_called = 0;
        o1.swap(o2);
        check(X::dtor_called == 1);
        check(!o1.has_value());
        check(o2.has_value());
        check(*o2 == 1);
    }
    {
        opt::option<X> o1;
        opt::option<X> o2(2);
        static_assert(noexcept(o1.swap(o2)));
        check(!o1.has_value());
        check(o2.has_value());
        check(*o2 == 2);
        X::dtor_called = 0;
        o1.swap(o2);
        check(X::dtor_called == 1);
        check(o1.has_value());
        check(*o1 == 2);
        check(!o2.has_value());
    }
    {
        opt::option<X> o1(1);
        opt::option<X> o2(2);
        static_assert(noexcept(o1.swap(o2)));
        check(o1.has_value());
        check(*o1 == 1);
        check(o2.has_value());
        check(*o2 == 2);
        X::dtor_called = 0;
        o1.swap(o2);
        check(X::dtor_called == 1); // from inside std::swap
        check(o1.has_value());
        check(*o1 == 2);
        check(o2.has_value());
        check(*o2 == 1);
    }
    {
        opt::option<Y> o1;
        opt::option<Y> o2;
        static_assert(!noexcept(o1.swap(o2)));
        check(!o1.has_value());
        check(!o2.has_value());
        o1.swap(o2);
        check(!o1.has_value());
        check(!o2.has_value());
        check(Y::dtor_called == 0);
    }
    {
        opt::option<Y> o1(1);
        opt::option<Y> o2;
        static_assert(!noexcept(o1.swap(o2)));
        check(o1.has_value());
        check(*o1 == 1);
        check(!o2.has_value());
        Y::dtor_called = 0;
        o1.swap(o2);
        check(Y::dtor_called == 1);
        check(!o1.has_value());
        check(o2.has_value());
        check(*o2 == 1);
    }
    {
        opt::option<Y> o1;
        opt::option<Y> o2(2);
        static_assert(!noexcept(o1.swap(o2)));
        check(!o1.has_value());
        check(o2.has_value());
        check(*o2 == 2);
        Y::dtor_called = 0;
        o1.swap(o2);
        check(Y::dtor_called == 1);
        check(o1.has_value());
        check(*o1 == 2);
        check(!o2.has_value());
    }
    {
        opt::option<Y> o1(1);
        opt::option<Y> o2(2);
        static_assert(!noexcept(o1.swap(o2)));
        check(o1.has_value());
        check(*o1 == 1);
        check(o2.has_value());
        check(*o2 == 2);
        Y::dtor_called = 0;
        o1.swap(o2);
        check(Y::dtor_called == 0);
        check(o1.has_value());
        check(*o1 == 2);
        check(o2.has_value());
        check(*o2 == 1);
    }
#if OPTION_HAS_EXCEPTIONS
    {
        opt::option<Z> o1;
        opt::option<Z> o2;
        static_assert(!noexcept(o1.swap(o2)));
        check(!o1.has_value());
        check(!o2.has_value());
        o1.swap(o2);
        check(!o1.has_value());
        check(!o2.has_value());
    }
#endif
#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
    {
        opt::option<Z> o1;
        o1.emplace(1);
        opt::option<Z> o2;
        static_assert(!noexcept(o1.swap(o2)));
        check(o1.has_value());
        check(*o1 == 1);
        check(!o2.has_value());
        check_throw_value(o1.swap(o2), int, 7);
        check(o1.has_value());
        check(*o1 == 1);
        check(!o2.has_value());
    }
    {
        opt::option<Z> opt1;
        opt::option<Z> opt2;
        opt2.emplace(2);
        static_assert(!noexcept(opt1.swap(opt2)));
        check(!opt1.has_value());
        check(opt2.has_value());
        check(*opt2 == 2);
        check_throw_value(opt1.swap(opt2), int, 7);
        check(!opt1.has_value());
        check(opt2.has_value());
        check(*opt2 == 2);
    }
    {
        opt::option<Z> o1;
        o1.emplace(1);
        opt::option<Z> o2;
        o2.emplace(2);
        static_assert(!noexcept(o1.swap(o2)));
        check(o1.has_value());
        check(*o1 == 1);
        check(o2.has_value());
        check(*o2 == 2);
        check_throw_value(o1.swap(o2), int, 6);
        check(o1.has_value());
        check(*o1 == 1);
        check(o2.has_value());
        check(*o2 == 2);
    }
#endif
    {
        opt::option<int> opt1;
        opt::option<int> opt2;
        static_assert(noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == false);
        check(opt2.has_value() == false);
        swap(opt1, opt2);
        check(opt1.has_value() == false);
        check(opt2.has_value() == false);
    }
    {
        opt::option<int> opt1(1);
        opt::option<int> opt2;
        static_assert(noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == false);
        swap(opt1, opt2);
        check(opt1.has_value() == false);
        check(opt2.has_value() == true);
        check(*opt2 == 1);
    }
    {
        opt::option<int> opt1;
        opt::option<int> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == false);
        check(opt2.has_value() == true);
        check(*opt2 == 2);
        swap(opt1, opt2);
        check(opt1.has_value() == true);
        check(*opt1 == 2);
        check(opt2.has_value() == false);
    }
    {
        opt::option<int> opt1(1);
        opt::option<int> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == true);
        check(*opt2 == 2);
        swap(opt1, opt2);
        check(opt1.has_value() == true);
        check(*opt1 == 2);
        check(opt2.has_value() == true);
        check(*opt2 == 1);
    }
    {
        opt::option<X> opt1;
        opt::option<X> opt2;
        static_assert(noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == false);
        check(opt2.has_value() == false);
        X::dtor_called = 0;
        swap(opt1, opt2);
        check(opt1.has_value() == false);
        check(opt2.has_value() == false);
        check(X::dtor_called == 0);
    }
    {
        opt::option<X> opt1(1);
        opt::option<X> opt2;
        static_assert(noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == false);
        X::dtor_called = 0;
        swap(opt1, opt2);
        check(X::dtor_called == 1);
        check(opt1.has_value() == false);
        check(opt2.has_value() == true);
        check(*opt2 == 1);
    }
    {
        opt::option<X> opt1;
        opt::option<X> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == false);
        check(opt2.has_value() == true);
        check(*opt2 == 2);
        X::dtor_called = 0;
        swap(opt1, opt2);
        check(X::dtor_called == 1);
        check(opt1.has_value() == true);
        check(*opt1 == 2);
        check(opt2.has_value() == false);
    }
    {
        opt::option<X> opt1(1);
        opt::option<X> opt2(2);
        static_assert(noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == true);
        check(*opt2 == 2);
        X::dtor_called = 0;
        swap(opt1, opt2);
        check(X::dtor_called == 1);  // from inside std::swap
        check(opt1.has_value() == true);
        check(*opt1 == 2);
        check(opt2.has_value() == true);
        check(*opt2 == 1);
    }
    {
        opt::option<Y> opt1;
        opt::option<Y> opt2;
        static_assert(!noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == false);
        check(opt2.has_value() == false);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        check(opt1.has_value() == false);
        check(opt2.has_value() == false);
        check(Y::dtor_called == 0);
    }
    {
        opt::option<Y> opt1(1);
        opt::option<Y> opt2;
        static_assert(!noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == false);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        check(Y::dtor_called == 1);
        check(opt1.has_value() == false);
        check(opt2.has_value() == true);
        check(*opt2 == 1);
    }
    {
        opt::option<Y> opt1;
        opt::option<Y> opt2(2);
        static_assert(!noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == false);
        check(opt2.has_value() == true);
        check(*opt2 == 2);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        check(Y::dtor_called == 1);
        check(opt1.has_value() == true);
        check(*opt1 == 2);
        check(opt2.has_value() == false);
    }
    {
        opt::option<Y> opt1(1);
        opt::option<Y> opt2(2);
        static_assert(!noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == true);
        check(*opt2 == 2);
        Y::dtor_called = 0;
        swap(opt1, opt2);
        check(Y::dtor_called == 0);
        check(opt1.has_value() == true);
        check(*opt1 == 2);
        check(opt2.has_value() == true);
        check(*opt2 == 1);
    }
    {
        opt::option<Z> opt1;
        opt::option<Z> opt2;
        static_assert(!noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == false);
        check(opt2.has_value() == false);
        swap(opt1, opt2);
        check(opt1.has_value() == false);
        check(opt2.has_value() == false);
    }
#if OPTION_HAS_EXCEPTIONS
    {
        opt::option<Z> opt1;
        opt1.emplace(1);
        opt::option<Z> opt2;
        static_assert(!noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == false);
        check_throw_value(swap(opt1, opt2), int, 7);

        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == false);
    }
    {
        opt::option<Z> opt1;
        opt::option<Z> opt2;
        opt2.emplace(2);
        static_assert(!noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == false);
        check(opt2.has_value() == true);
        check(*opt2 == 2);

        check_throw_value(swap(opt1, opt2), int, 7);

        check(opt1.has_value() == false);
        check(opt2.has_value() == true);
        check(*opt2 == 2);
    }
    {
        opt::option<Z> opt1;
        opt1.emplace(1);
        opt::option<Z> opt2;
        opt2.emplace(2);
        static_assert(!noexcept(swap(opt1, opt2)));
        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == true);
        check(*opt2 == 2);

        check_throw_value(swap(opt1, opt2), int, 6);

        check(opt1.has_value() == true);
        check(*opt1 == 1);
        check(opt2.has_value() == true);
        check(*opt2 == 2);
    }
#endif
}
