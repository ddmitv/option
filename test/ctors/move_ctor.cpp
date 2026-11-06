#include <opt/option.hpp>

#include <ctors/test_types.hpp>
#include <tests.hpp>
#include <type_traits>
#include <utility>

// option(option&&)

namespace {

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/move.pass.cpp

OPTION_GCC_SUPPRESS_WARNING("-Wmaybe-uninitialized")

template<typename T, typename... InitArgs>
void test(InitArgs&&... args) {
    const opt::option<T> orig(std::forward<InitArgs>(args)...);
    opt::option<T> rhs(orig);
    const bool rhs_engaged = rhs.has_value();
    opt::option<T> lhs = std::move(rhs);
    check(lhs.has_value() == rhs_engaged);
    if (rhs_engaged) {
        SUPPRESS_WCONSUMED(
        check(*lhs == *orig);
        )
    }
}

template<typename T, typename... InitArgs>
constexpr bool constexpr_test(InitArgs&&... args) {
    static_assert(std::is_trivially_copy_constructible_v<T>);
    const opt::option<T> orig(std::forward<InitArgs>(args)...);
    opt::option<T> rhs(orig);
    opt::option<T> lhs = std::move(rhs);
    check(lhs.has_value() == orig.has_value());
    if (lhs.has_value()) {
        check(orig.has_value() && *lhs == *orig);
    }
    return true;
}

void test_throwing_ctor() {
#if OPTION_HAS_EXCEPTIONS && !GCC_HWASAN_UNWINDER_BROKEN
    struct Z {
        Z() : count(0) {}
        Z(Z&& o) : count(o.count + 1) {
            if (count == 2) {
                throw 6;
            }
        }
        int count;
    };
    Z z;
    opt::option<Z> rhs(std::move(z));
    try {
        opt::option<Z> lhs(std::move(rhs));
        check(false);
    } catch (int i) {
        check(i == 6);
    }
#endif
}

// TODO: test_ref

}

void move_ctor_test_suite() {
    test<int>();
    test<int>(3);
    static_assert(constexpr_test<int>());
    static_assert(constexpr_test<int>(3));

    {
        opt::option<const int> o(42);
        opt::option<const int> o2(std::move(o));
        check(*o2 == 42);
    }
    {
        using T = test_types::test_type;
        T::reset();
        opt::option<T> rhs;
        check(T::alive == 0);
        opt::option<T> lhs(std::move(rhs));
        check(lhs.has_value() == false);
        check(rhs.has_value() == false);
        check(T::alive == 0);
    }
    test_types::test_type::reset();
    {
        using T = test_types::test_type;
        T::reset();
        opt::option<T> rhs(42);
        check(T::alive == 1);
        check(T::value_constructed == 1);
        check(T::move_constructed == 0);
        opt::option<T> lhs(std::move(rhs));
        check(lhs.has_value());
        check(rhs.has_value());
        check(lhs.get().value == 42);
        SUPPRESS_WCONSUMED (
            check(rhs.get().value == -1);
        )
        check(T::move_constructed == 1);
        check(T::alive == 2);
    }
    test_types::test_type::reset();
    {
        test<test_types::constexpr_test_type>();
        test<test_types::constexpr_test_type>(42);
    }
    {
        test<test_types::triv_test_type>();
        test<test_types::triv_test_type>(42);
    }
    {
        test_throwing_ctor();
    }
    {
        struct ThrowsMove {
            [[maybe_unused]] ThrowsMove() noexcept(false) {}
            [[maybe_unused]] ThrowsMove(ThrowsMove const&) noexcept(false) {}
            [[maybe_unused]] ThrowsMove(ThrowsMove&&) noexcept(false) {}
        };
        static_assert(!std::is_nothrow_move_constructible_v<opt::option<ThrowsMove>>);
        struct NoThrowMove {
            [[maybe_unused]] NoThrowMove() noexcept(false) {}
            [[maybe_unused]] NoThrowMove(NoThrowMove const&) noexcept(false) {}
            [[maybe_unused]] NoThrowMove(NoThrowMove&&) noexcept(true) {}
        };
        static_assert(std::is_nothrow_move_constructible_v<opt::option<NoThrowMove>>);
    }
    {
        constexpr opt::option<int> o1{4};
        constexpr opt::option<int> o2 = std::move(o1);
        static_assert(*o2 == 4);
    }
}
