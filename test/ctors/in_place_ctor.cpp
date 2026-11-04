#include <opt/option.hpp>

#include <tests.hpp>
#include <initializer_list>
#include <memory>
#include <type_traits>
#include <vector>

// template<typename... Args>
// option(in_place_t, Args&&... args)
// template<typename U, typename... Args>
// explicit option(in_place_t, initializer_list<U> il, Args&&... args)

namespace {

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/in_place_t.pass.cpp
// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/initializer_list.pass.cpp

class X {
    int i_;
    int j_ = 0;
public:
    X() : i_(0) {}
    X(int i) : i_(i) {}
    X(int i, int j) : i_(i), j_(j) {}

    ~X() {}

    friend bool operator==(const X& x, const X& y) { return x.i_ == y.i_ && x.j_ == y.j_; }
};

class Y {
    int i_;
    int j_ = 0;
public:
    constexpr Y() : i_(0) {}
    constexpr Y(int i) : i_(i) {}
    constexpr Y(int i, int j) : i_(i), j_(j) {}
    constexpr Y(std::initializer_list<int> il) : i_(il.begin()[0]), j_(il.begin()[1]) {}

    friend constexpr bool operator==(const Y& x, const Y& y) { return x.i_ == y.i_ && x.j_ == y.j_; }
};

class Z {
public:
    [[noreturn]] Z(int) { throw 6; }
    [[noreturn]] Z(std::initializer_list<int>) { throw 6; }
};

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/gh_101960_internal_ctor.compile.pass.cpp
struct nasty_convertible {
    template <class T>
    [[maybe_unused]] operator T() {
        return 0;
    }
};

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/empty_in_place_t_does_not_clobber.pass.cpp
struct Wrapped {
    struct Inner {
        int dummy = 42;
    };
    opt::option<Inner> inner;
};

}

void in_place_ctor_test_suite() {
    {
        constexpr opt::option<int> opt(std::in_place, 5);
        static_assert(opt.has_value());
        static_assert(*opt == 5);

        struct test_constexpr_ctor : opt::option<int> {
            [[maybe_unused]] constexpr test_constexpr_ctor(std::in_place_t, int i) : option(std::in_place, i) {}
        };
    }
    {
        opt::option<const int> opt(std::in_place, 5);
        check(*opt == 5);
    }
    {
        const opt::option<X> opt(std::in_place);
        check(opt.has_value() == true);
        check(*opt == X());
    }
    {
        const opt::option<X> opt(std::in_place, 5);
        check(opt.has_value() == true);
        check(*opt == X(5));
    }
    {
        const opt::option<X> opt(std::in_place, 5, 4);
        check(opt.has_value() == true);
        check(*opt == X(5, 4));
    }
    {
        constexpr opt::option<Y> opt(std::in_place);
        static_assert(opt.has_value());
        static_assert(*opt == Y());

        struct test_constexpr_ctor : opt::option<Y> {
            [[maybe_unused]] constexpr test_constexpr_ctor(std::in_place_t) : option(std::in_place) {}
        };
    }
    {
        constexpr opt::option<Y> opt(std::in_place, 5);
        static_assert(opt.has_value());
        static_assert(*opt == Y(5));

        struct test_constexpr_ctor : opt::option<Y> {
            [[maybe_unused]] constexpr test_constexpr_ctor(std::in_place_t, int i) : option(std::in_place, i) {}
        };
    }
    {
        constexpr opt::option<Y> opt(std::in_place, 5, 4);
        static_assert(opt.has_value());
        static_assert(*opt == Y(5, 4));

        struct test_constexpr_ctor : opt::option<Y> {
            [[maybe_unused]] constexpr test_constexpr_ctor(std::in_place_t, int i, int j) : option(std::in_place, i, j) {}
        };
    }
#if OPTION_HAS_EXCEPTIONS
    {
        try {
            const opt::option<Z> opt(std::in_place, 1);
            check(false);
        } catch (int i) {
            check(i == 6);
        }
    }
#endif
    {
        static_assert(!std::is_constructible_v<X, std::initializer_list<int>&>);
        static_assert(!std::is_constructible_v<opt::option<X>, std::initializer_list<int>&>);
    }
    {
        opt::option<std::vector<int>> opt(std::in_place, {3, 1});
        check(opt.has_value() == true);
        check((*opt == std::vector<int>{3, 1}));
        check(opt->size() == 2);
    }
    {
        opt::option<std::vector<int>> opt(std::in_place, {3, 1}, std::allocator<int>());
        check(opt.has_value() == true);
        check((*opt == std::vector<int>{3, 1}));
        check(opt->size() == 2);
    }
    {
        static_assert(std::is_constructible_v<opt::option<Y>, std::initializer_list<int>&>);
        constexpr opt::option<Y> opt(std::in_place, {3, 1});
        static_assert(opt.has_value());
        static_assert(*opt == Y{3, 1});

        struct test_constexpr_ctor : opt::option<Y> {
            [[maybe_unused]] constexpr test_constexpr_ctor(std::in_place_t, std::initializer_list<int> i) : option(std::in_place, i) {}
        };
    }
#if OPTION_HAS_EXCEPTIONS
    {
        static_assert(std::is_constructible_v<opt::option<Z>, std::initializer_list<int>&>);
        try {
            opt::option<Z> opt(std::in_place, {3, 1});
            check(false);
        } catch (int i) {
            check(i == 6);
        }
    }
#endif
    // TODO: test_ref

    static_assert(!std::is_constructible_v<opt::option<int>, nasty_convertible, int(int), int>);
    {
        static_assert(std::is_default_constructible_v<Wrapped::Inner>);
        Wrapped w;
        w.inner.emplace();
        check(w.inner.has_value());
        check(w.inner->dummy == 42);
    }
}
