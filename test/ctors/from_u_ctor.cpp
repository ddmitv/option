
#include <opt/option.hpp>

#include <ctors/test_types.hpp>
#include <tests.hpp>
#include <type_traits>
#include <vector>

namespace {

struct implicit_throw {
    constexpr implicit_throw(int x) {
        if (x != -1) {
            throw 6;
        }
    }
};

struct explicit_throw {
    constexpr explicit explicit_throw(int x) {
        if (x != -1) {
            throw 6;
        }
    }
};

struct implicit_any {
    template <class U>
    [[maybe_unused]] constexpr implicit_any(U&&) {}
};

struct implicit_no_throw {
    [[maybe_unused]] constexpr implicit_no_throw(int) noexcept {}
};
struct explicit_no_throw {
    [[maybe_unused]] constexpr explicit explicit_no_throw(int) noexcept {}
};

template<typename T>
[[maybe_unused]] T prvalue() noexcept {
    static_assert(false);
}

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/support/test_convertible.h
template<typename T, typename... Args>
concept is_implicitly_constructible = requires(void (*consumer)(T)) {
    consumer({prvalue<Args>()...});
};

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/U.pass.cpp

template<typename To, typename From>
constexpr bool implicit_conversion(opt::option<To>&& o, const From& v) {
    static_assert(is_implicitly_constructible<opt::option<To>, From>);
    static_assert(!is_implicitly_constructible<opt::option<To>, void*>);
    static_assert(!is_implicitly_constructible<opt::option<To>, From, int>);
    return o.has_value() && o.get() == static_cast<To>(v);
}

template<typename To, typename Input, typename Expect>
constexpr bool explicit_conversion(Input&& in, const Expect& v) {
    static_assert(std::is_constructible_v<opt::option<To>, Input>);
    static_assert(!std::is_convertible_v<Input, opt::option<To>>);
    static_assert(!std::is_constructible_v<opt::option<To>, void*>);
    static_assert(!std::is_constructible_v<opt::option<To>, Input, int>);
    opt::option<To> o1(std::forward<Input>(in));
    opt::option<To> o2{std::forward<Input>(in)};
    return o1.has_value() && o1.get() == static_cast<To>(v) && o2.has_value() && o2.get() == static_cast<To>(v);
}

void test_implicit() {
    static_assert(implicit_conversion<long long>(42, 42));
    static_assert(implicit_conversion<long double>(3.14, 3.14));

    {
        int x = 42;
        opt::option<void* const> o(&x);
        check(o.get() == &x);
    }
    static_assert(implicit_conversion<test_types::triv_test_type>(42, 42));
    check(implicit_conversion<test_types::test_type>(3, test_types::test_type(3)));
    {
        opt::option<test_types::test_type> o({3});
        check(o.has_value() && o.get() == static_cast<test_types::test_type>(3));
    }
    static_assert(!is_implicitly_constructible<opt::option<implicit_any>, std::in_place_t>);
    static_assert(!is_implicitly_constructible<opt::option<implicit_any>, std::in_place_t&>);
    static_assert(!is_implicitly_constructible<opt::option<implicit_any>, const std::in_place_t&>);
    static_assert(!is_implicitly_constructible<opt::option<implicit_any>, std::in_place_t&&>);
    static_assert(!is_implicitly_constructible<opt::option<implicit_any>, const std::in_place_t&&>);

    static_assert(!noexcept(opt::option<implicit_no_throw>(42)));
    static_assert(!noexcept(opt::option<implicit_throw>(42)));

    check_throw([[maybe_unused]] opt::option<implicit_throw> o = 42, int);
}

void test_explicit() {
    static_assert(explicit_conversion<test_types::explicit_triv_test_type>(42, 42));
    static_assert(explicit_conversion<test_types::explicit_constexpr_test_type>(42, 42));
    static_assert(!std::is_convertible_v<int, test_types::explicit_constexpr_test_type>);

    {
        using T = test_types::explicit_test_type;
        T::reset();
        {
            check(explicit_conversion<T>(42, 42));
            check(T::alive == 0);
        }
        T::reset();
        {
            opt::option<T> t(42);
            check(T::alive == 1);
            check(T::value_constructed == 1);
            check(T::move_constructed == 0);
            check(T::copy_constructed == 0);
            check(t.get().value == 42);
        }
        check(T::alive == 0);
    }
    static_assert(!noexcept(opt::option<explicit_no_throw>(42)));
    static_assert(!noexcept(opt::option<explicit_throw>(42)));

    static_assert(!std::is_nothrow_constructible_v<opt::option<std::vector<int>>, std::vector<int>&&>);
    check_throw([[maybe_unused]] opt::option<explicit_throw> o(42), int);
}

void test_const_T_ctor() {
    {
        constexpr int x = 5;
        constexpr opt::option<int> o(x);
        static_assert(o.has_value() == true);
        static_assert(o.get() == 5);
    }
    {
        constexpr double x = 3.0;
        constexpr opt::option<double> o(x);
        static_assert(o.has_value() == true);
        static_assert(o.get() == 3);
    }
    {
        const int x = 42;
        opt::option<const int> o(x);
        check(o.get() == x);
    }
    {
        using T = test_types::test_type;
        T::reset();

        const T x(3);
        opt::option<T> o = x;
        check(T::alive == 2);
        check(T::copy_constructed == 1);
        check(o.has_value() == true);
        check(o.get().value == 3);
    }
    {
        using T = test_types::explicit_test_type;
        static_assert(!std::is_convertible_v<const T&, opt::option<T>>);
        T::reset();

        const T x(3);
        opt::option<T> o(x);
        check(T::alive == 2);
        check(T::copy_constructed == 1);
        check(o.has_value() == true);
        check(o.get().value == 3);
    }
    {
        using T = test_types::constexpr_test_type;
        constexpr T x(3);
        constexpr opt::option<T> o = {x};
        static_assert(o.has_value() == true);
        static_assert(o.get().value == 3);
    }
    {
        using T = test_types::explicit_constexpr_test_type;
        static_assert(!std::is_convertible_v<const T&, opt::option<T>>);
        constexpr T x(3);
        constexpr opt::option<T> o(x);
        static_assert(o.has_value() == true);
        static_assert(o.get().value == 3);
    }
    {
        struct Z {
            Z(int) {}
            [[noreturn]] Z(const Z&) { throw 6; }
        };
        const Z x(3);
        check_throw_value([[maybe_unused]] opt::option<Z> o(x), int, 6);
    }
}

// test ref https://github.com/llvm/llvm-project/blob/74d6edc53625fb0b8652cd928ef7bdca95570aa6/libcxx/test/std/utilities/optional/optional.object/optional.object.ctor/U.pass.cpp#L175

}

void from_u_ctor_test_suite() {
    test_implicit();
    test_explicit();
    test_const_T_ctor();
}
