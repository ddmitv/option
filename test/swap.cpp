#include <tests.hpp>
#include <opt/option.hpp>

#include <type_traits>
#include <utility>

// void swap(option& other)
//     noexcept(is_nothrow_move_constructible<T>::value &&
//              is_nothrow_swappable<T>::value)

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.swap/swap.pass.cpp

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

#if OPTION_HAS_EXCEPTIONS
class Z {
    int i;
public:
    Z(int i_) : i(i_) {}
    [[noreturn]] Z(Z&&) { throw 7; }

    friend constexpr bool operator==(const Z& x, const Z& y) { return x.i == y.i; }
    [[noreturn]] friend void swap(Z&, Z&) { throw 6; }
};
#endif

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

}

void swap_test_suite() {
    check_swap<int>();
    check_swap<W>();
    static_assert(check_swap<int>());
    static_assert(check_swap<W>());

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
}
