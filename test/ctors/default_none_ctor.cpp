
#include <opt/option.hpp>

#include <ctors/test_types.hpp>
#include <tests.hpp>
#include <type_traits>

namespace {

template<typename T>
void test_constexpr() {
    static_assert(std::is_trivially_destructible_v<opt::option<T>>);
    static_assert(std::is_trivially_destructible_v<T>);
    {
        static_assert(std::is_nothrow_default_constructible_v<opt::option<T>>);

        constexpr opt::option<T> o;
        static_assert(o.has_value() == false);
    }
    {
        static_assert(std::is_nothrow_constructible_v<opt::option<T>, opt::none_t&>);

        constexpr opt::option<T> o(opt::none);
        static_assert(o.has_value() == false);
    }
}

template<typename T>
void test_non_triv_dtor() {
    static_assert(!std::is_trivially_destructible_v<opt::option<T>>);
    static_assert(!std::is_trivially_destructible_v<T>);
    {
        static_assert(std::is_nothrow_default_constructible_v<opt::option<T>>);
        {
            opt::option<T> o;
            check(o.has_value() == false);
        }
        {
            const opt::option<T> o;
            check(o.has_value() == false);
        }
        // check for constexpr-ness of opt::option ctor for non trivially dtor types
        struct test_constexpr_ctor {
            opt::option<T> o;
            constexpr test_constexpr_ctor() {}
        };
    }
    {
        static_assert(std::is_nothrow_constructible_v<opt::option<T>, opt::none_t&>);
        {
            opt::option<T> o(opt::none);
            check(o.has_value() == false);
        }
        {
            const opt::option<T> o(opt::none);
            check(o.has_value() == false);
        }
    }
}

}

void default_none_ctor_test_suite() {
    test_constexpr<int>();
    test_constexpr<int*>();
    test_constexpr<test_types::no_ctors>();
    test_constexpr<test_types::explicit_no_ctors>();

    test_non_triv_dtor<test_types::non_triv_dtor_no_ctors>();

    // TODO: add tests for opt::option<T&>
}
