
#include <opt/option.hpp>

#include <ctors/test_types.hpp>
#include <tests.hpp>
#include <type_traits>
#include <vector>

// option(const option<T>&)

OPTION_GCC_SUPPRESS_WARNING("-Wmaybe-uninitialized")

namespace {

template<typename T, typename... InitArgs>
void test(InitArgs&&... args) {
    const opt::option<T> rhs(std::forward<InitArgs>(args)...);
    opt::option<T> lhs = rhs;
    check(lhs.has_value() == rhs.has_value());
    if (rhs.has_value()) {
        check(*lhs == *rhs);
    }
}

template<typename T, typename... InitArgs>
constexpr bool constexpr_test(InitArgs&&... args) {
    static_assert(std::is_trivially_copy_constructible_v<T>);
    const opt::option<T> rhs(std::forward<InitArgs>(args)...);
    opt::option<T> lhs = rhs;
    check(lhs.has_value() == rhs.has_value());
    if (lhs.has_value()) {
        check(rhs.has_value() && *lhs == *rhs);
    }
    return true;
}

void test_throwing_ctor() {
#if 1
    struct Z {
        Z() : count(0) {}
        Z(Z const& o) : count(o.count + 1) {
            if (count == 2) {
                throw 6;
            }
        }
        int count;
    };
    const Z z;
    const opt::option<Z> rhs(z);
    try {
        opt::option<Z> lhs(rhs);
        check(false);
    } catch (int i) {
        check(i == 6);
    }
#endif
}

// TODO: test_ref

}

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/copy.pass.cpp
void copy_ctor_test_suite() {
    test<int>();
    test<int>(3);
    static_assert(constexpr_test<int>());
    static_assert(constexpr_test<int>(3));

    {
        const opt::option<const int> o(42);
        opt::option<const int> o2(o);
        check(*o2 == 42);
    }
    {
        using T = test_types::test_type;
        T::reset();
        const opt::option<T> rhs;
        check(T::alive == 0);
        const opt::option<T> lhs(rhs);
        check(lhs.has_value() == false);
        check(T::alive == 0);
    }
    test_types::test_type::reset();
    {
        using T = test_types::test_type;
        T::reset();
        const opt::option<T> rhs(42);
        check(T::alive == 1);
        check(T::value_constructed == 1);
        check(T::copy_constructed == 0);
        const opt::option<T> lhs(rhs);
        check(lhs.has_value());
        check(T::copy_constructed == 1);
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
        constexpr opt::option<int> o1{4};
        constexpr opt::option<int> o2 = o1;
        static_assert(*o2 == 4);
    }

    // LWG3836 https://wg21.link/LWG3836
    // std::optional<bool> conversion constructor optional(const optional<U>&)
    // should take precedence over optional(U&&) with operator bool
    {
        opt::option<bool> o1(false);
        opt::option<bool> o2(o1);
        check(!o2.value());
    }
}
