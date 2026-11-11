#include <opt/option.hpp>

#include <archetypes.hpp>
#include <tests.hpp>
#include <type_traits>
#include <utility>

// constexpr option& operator=(option&& rhs)
//     noexcept(is_nothrow_move_assignable<T>::value &&
//              is_nothrow_move_constructible<T>::value);

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.assign/move.pass.cpp

namespace {

#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
struct X {
    static inline bool throw_now = false;
    static inline int alive = 0;

    X() { ++alive; }
    X(X&&) {
        if (throw_now) {
            throw 6;
        }
        ++alive;
    }
    X& operator=(X&&) {
        if (throw_now) {
            throw 42;
        }
        return *this;
    }
    ~X() {
        check(alive > 0);
        --alive;
    }
};
#endif

struct Y {};

template<typename Tp>
constexpr bool assign_empty(opt::option<Tp>&& lhs) {
    opt::option<Tp> rhs;
    lhs = std::move(rhs);
    return !lhs.has_value() && !rhs.has_value();
}

template<typename Tp>
constexpr bool assign_value(opt::option<Tp>&& lhs) {
    opt::option<Tp> rhs(101);
    lhs = std::move(rhs);
    return lhs.has_value() && rhs.has_value() && *lhs == Tp{101};
}

// TODO: test_ref

}

void move_assign_test_suite() {
    {
        static_assert(std::is_nothrow_move_assignable_v<opt::option<int>>);
        opt::option<int> opt;
        constexpr opt::option<int> opt2;
        opt = std::move(opt2);
        static_assert(!opt2.has_value());
        check(opt.has_value() == opt2.has_value());
    }
    {
        opt::option<int> opt;
        constexpr opt::option<int> opt2(2);
        opt = std::move(opt2);
        static_assert(opt2.has_value());
        static_assert(*opt2 == 2);
        check(opt.has_value() == opt2.has_value());
        SUPPRESS_WCONSUMED(
        check(*opt == *opt2);
        )
    }
    {
        opt::option<int> opt(3);
        constexpr opt::option<int> opt2;
        opt = std::move(opt2);
        static_assert(!opt2.has_value());
        check(opt.has_value() == opt2.has_value());
    }
    {
        using T = test_types::test_type;
        T::reset();
        opt::option<T> opt(3);
        opt::option<T> opt2;
        check(T::alive == 1);
        opt = std::move(opt2);
        check(T::alive == 0);
        check(!opt2.has_value());
        check(opt.has_value() == opt2.has_value());
    }
    test_types::test_type::reset();
    {
        opt::option<int> opt(3);
        constexpr opt::option<int> opt2(2);
        opt = std::move(opt2);
        static_assert(opt2.has_value());
        static_assert(*opt2 == 2);
        check(opt.has_value() == opt2.has_value());
        SUPPRESS_WCONSUMED(
        check(*opt == *opt2);
        )
    }
    {
        using O = opt::option<int>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        check(assign_empty(O{42}));
        check(assign_value(O{42}));
    }
    {
        using O = opt::option<trivial_test_types::all_ctors>;
        static_assert(assign_empty(O{42}));
        static_assert(assign_value(O{42}));
        check(assign_empty(O{42}));
        check(assign_value(O{42}));
    }
#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
    {
        static_assert(!std::is_nothrow_move_assignable_v<opt::option<X>>);
        X::alive = 0;
        X::throw_now = false;
        opt::option<X> opt;
        opt::option<X> opt2(X{});
        check(X::alive == 1);
        check(opt2.has_value());
        X::throw_now = true;
        check_throw_value(opt = std::move(opt2), int, 6);
        check(!opt.has_value());
        check(X::alive == 1);
    }
    check(X::alive == 0);
    {
        static_assert(!std::is_nothrow_move_assignable_v<opt::option<X>>);
        X::throw_now = false;
        opt::option<X> opt(X{});
        opt::option<X> opt2(X{});
        check(X::alive == 2);
        check(opt2.has_value());
        X::throw_now = true;
        check_throw_value(opt = std::move(opt2), int, 42);
        check(opt.has_value());
        check(X::alive == 2);
    }
    check(X::alive == 0);
#endif
    {
        static_assert(std::is_nothrow_move_assignable_v<opt::option<Y>>);
    }
    {
        struct ThrowsMove {
            [[maybe_unused]] ThrowsMove() noexcept {}
            [[maybe_unused]] ThrowsMove(ThrowsMove const&) noexcept {}
            [[maybe_unused]] ThrowsMove(ThrowsMove&&) noexcept(false) {}
            [[maybe_unused]] ThrowsMove& operator=(ThrowsMove const&) noexcept { return *this; }
            [[maybe_unused]] ThrowsMove& operator=(ThrowsMove&&) noexcept { return *this; }
        };
        static_assert(!std::is_nothrow_move_assignable_v<opt::option<ThrowsMove>>);

        struct ThrowsMoveAssign {
            [[maybe_unused]] ThrowsMoveAssign() noexcept {}
            [[maybe_unused]] ThrowsMoveAssign(ThrowsMoveAssign const&) noexcept {}
            [[maybe_unused]] ThrowsMoveAssign(ThrowsMoveAssign&&) noexcept {}
            [[maybe_unused]] ThrowsMoveAssign& operator=(ThrowsMoveAssign const&) noexcept { return *this; }
            [[maybe_unused]] ThrowsMoveAssign& operator=(ThrowsMoveAssign&&) noexcept(false) { return *this; }
        };
        static_assert(!std::is_nothrow_move_assignable_v<opt::option<ThrowsMoveAssign>>);

        struct NoThrowMove {
            [[maybe_unused]] NoThrowMove() noexcept(false) {}
            [[maybe_unused]] NoThrowMove(NoThrowMove const&) noexcept(false) {}
            [[maybe_unused]] NoThrowMove(NoThrowMove&&) noexcept {}
            [[maybe_unused]] NoThrowMove& operator=(NoThrowMove const&) noexcept { return *this; }
            [[maybe_unused]] NoThrowMove& operator=(NoThrowMove&&) noexcept { return *this; }
        };
        static_assert(std::is_nothrow_move_assignable_v<opt::option<NoThrowMove>>);
    }
}
