// Copyright 2025 Dima Dmitriev.
// Distributed under the Boost Software License, Version 1.0. (See
// accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)

#include <opt/option.hpp>
#include <utility>
#include <variant>
#include <string>
#include <vector>
#include <cstdint>
#include <memory>

#include "tests.hpp"

OPTION_SUPPRESS_COMMON_WARNINGS_PUSH

#if OPTION_INSERT_ANNONATION
    #define asan_check_poisoned(T, addr) \
        if constexpr (!OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)) check(::__asan_address_is_poisoned((addr)) != 0)
    #define asan_check_unpoisoned(T, addr) \
        if constexpr (!OPTION_IS_TRIVIALLY_DESTRUCTIBLE(T)) check(::__asan_address_is_poisoned((addr)) == 0)
#else
    #define asan_check_poisoned(T, addr) (void)addr
    #define asan_check_unpoisoned(T, addr) (void)addr
#endif

namespace {

struct some_struct {
    constexpr some_struct(int a, float b, char c) : a{a}, b{b}, c{c} {}

    constexpr auto operator<=>(const some_struct&) const = default;

    int a = 2;
    float b = 34.3f;
    char c = 'b';
};

// std::variant produces false-positives on TypeSanitizer
template<typename T> OPTION_NO_SANITIZE("type")
void test_address_sanitizer(const std::remove_cv_t<T>& first_value, const std::remove_cv_t<T>& second_value) {
    {
        std::variant<opt::option<T>, int> var{std::in_place_index<0>, first_value};
        asan_check_unpoisoned(T, &var);
        check(std::get<0>(var).get() == first_value);

        std::get<0>(var).reset();
        asan_check_poisoned(T, &var);
        check(!std::get<0>(var).has_value());

        var.template emplace<1>(563);
        asan_check_unpoisoned(T, &var);
        check(std::get<1>(var) == 563);
    }
    {
        opt::option<T> o;
        asan_check_poisoned(T, &o);
        check(!o.has_value());

        [[maybe_unused]] auto&& _1 = o.emplace(second_value);
        asan_check_unpoisoned(T, &o);
        check(o.get() == second_value);
    }
    {
        opt::option<T> o{second_value};
        asan_check_unpoisoned(T, &o);
        check(o.get() == second_value);

        o.reset();
        asan_check_poisoned(T, &o);
        check(!o.has_value());
    }
    { // option(const option&)
        opt::option<T> o1{first_value};
        asan_check_unpoisoned(T, &o1);
        opt::option<T> o2{o1};
        asan_check_unpoisoned(T, &o2);

        opt::option<T> o3;
        asan_check_poisoned(T, &o3);
        opt::option<T> o4{o3};
        asan_check_poisoned(T, &o4);
    }
}

template<typename T>
void test_generic_const_opt_option(const std::remove_cv_t<T>& first_value, [[maybe_unused]] const std::remove_cv_t<T>& second_value) {
    { // constructors
        { // option()/option(none_t)
            {
                opt::option<T> o;
                check(!o.has_value());
            }
            {
                opt::option<T> o(opt::none);
                check(!o.has_value());
            }
            {
                opt::option<T> o = opt::none;
                check(!o.has_value());
            }
        }
        { // option(U&&)
            opt::option<T> o(first_value);
            check(o.get() == first_value);
        }
        { // option(const option&)
            {
                opt::option<T> o1{first_value};
                check(o1.get() == first_value);
                opt::option<T> o2{o1};
                check(o2.get() == first_value);
                check(o1.get() == first_value);
            }
            {
                opt::option<T> o1;
                check(!o1.has_value());
                opt::option<T> o2{o1};
                check(!o2.has_value());
                check(!o1.has_value());
            }
        }
        { // option(option&&)
            {
                opt::option<T> o1{first_value};
                check(o1.get() == first_value);
                opt::option<T> o2{std::move(o1)};
                check(o2.get() == first_value);
                check(o1.has_value());
            }
            {
                opt::option<T> o1;
                check(!o1.has_value());
                opt::option<T> o2{std::move(o1)};
                check(!o2.has_value());
                check(!o1.has_value());
            }
        }
        { // option(in_place_t, Args&&...)
            opt::option<T> o1{std::in_place, first_value};
            check(o1.get() == first_value);
        }
    }
    { // get()
        opt::option<T> o{first_value};
        check(o.get() == first_value);
        check(std::as_const(o).get() == first_value);
        check(std::move(o).get() == first_value);
        check(std::move(std::as_const(o)).get() == first_value);

        static_assert(std::is_same_v<decltype(o.get()), T&>);
        static_assert(std::is_same_v<decltype(std::as_const(o).get()), const T&>);
        static_assert(std::is_same_v<decltype(std::move(o).get()), T&&>);
        static_assert(std::is_same_v<decltype(std::move(std::as_const(o)).get()), const T&&>);
    }
    { // operator->
        opt::option<T> o{first_value};
        check(o.operator->() == &o.get());
        check(std::as_const(o).operator->() == &std::as_const(o).get());

        static_assert(std::is_same_v<decltype(o.operator->()), T*>);
        static_assert(std::is_same_v<decltype(std::as_const(o).operator->()), const T*>);
    }
    { // operator*
        opt::option<T> o{first_value};
        check(*o == first_value);
        check(*std::as_const(o) == first_value);
        check(*std::move(o) == first_value);
        check(*std::move(std::as_const(o)) == first_value);

        static_assert(std::is_same_v<decltype(*o), T&>);
        static_assert(std::is_same_v<decltype(*std::as_const(o)), const T&>);
        static_assert(std::is_same_v<decltype(*std::move(o)), T&&>);
        static_assert(std::is_same_v<decltype(*std::move(std::as_const(o))), const T&&>);
    }
    { // operators
        // https://github.com/llvm/llvm-project/blob/0e802c072a9f7f7845d83e57fd1baea9f5b1cad3/libcxx/test/std/utilities/optional/optional.relops/equal.pass.cpp
        opt::option<T> o1;
        opt::option<T> o2 = first_value;
        opt::option<T> o3 = second_value;

        { // operator==(const option<T1>&, const option<T2>&)
            check(o1 == o1);
            check(!(o1 == o2));
            check(!(o1 == o3));
            check(o2 == o2);
            check(!(o2 == o3));
            check(o3 == o3);
        }
        { // operator!=(const option<T1>&, const option<T2>&)
            check(!(o1 != o1));
            check(o1 != o2);
            check(o1 != o3);
            check(!(o2 != o2));
            check(o2 != o3);
            check(!(o3 != o3));
        }
        { // operator<(const option<T1>&, const option<T2>&)
            check(!(o1 < o1));
            check(o1 < o2);
            check(o1 < o3);
            check(!(o2 < o1));
            check(!(o2 < o2));
            check((o2 < o3) == (first_value < second_value));
            check(!(o3 < o1));
            check((o3 < o2) == (second_value < first_value));
            check(!(o3 < o3));
        }
        { // operator>(const option<T1>&, const option<T2>&)
            check(!(o1 > o1));
            check(!(o1 > o2));
            check(!(o1 > o3));
            check(o2 > o1);
            check(!(o2 > o2));
            check((o2 > o3) == (first_value > second_value));
            check(o3 > o1);
            check((o3 > o2) == (second_value > first_value));
            check(!(o3 > o3));
        }
        { // operator<=(const option<T1>&, const option<T2>&)
            check(o1 <= o1);
            check(o1 <= o2);
            check(o1 <= o3);
            check(!(o2 <= o1));
            check(o2 <= o2);
            check((o2 <= o3) == (first_value <= second_value));
            check(!(o3 <= o1));
            check((o3 <= o2) == (second_value <= first_value));
            check(o3 <= o3);
        }
        { // operator>=(const option<T1>&, const option<T2>&)
            check(o1 >= o1);
            check(!(o1 >= o2));
            check(!(o1 >= o3));
            check(o2 >= o1);
            check(o2 >= o2);
            check((o2 >= o3) == (first_value >= second_value));
            check(o3 >= o1);
            check((o3 >= o2) == (second_value >= first_value));
            check(o3 >= o3);
        }
        // operator<=>(const option<T1>&, const option<T2>&)
        if constexpr (std::three_way_comparable_with<T, T>) {
            check((o1 <=> o1) == std::strong_ordering::equal);
            check((o1 <=> o2) == std::strong_ordering::less);
            check((o1 <=> o3) == std::strong_ordering::less);
            check((o2 <=> o1) == std::strong_ordering::greater);
            check((o2 <=> o2) == std::strong_ordering::equal);
            check((o2 <=> o3) == (first_value <=> second_value));
            check((o3 <=> o1) == std::strong_ordering::greater);
            check((o3 <=> o2) == (second_value <=> first_value));
            check((o3 <=> o3) == std::strong_ordering::equal);
        }
        { // operator==(const option<T>&, const none_t)
            check(o1 == opt::none);
            check(!(o2 == opt::none));
            check(!(o3 == opt::none));
        }
        { // operator<=>(const option<T>&, const none_t)
            check((o1 <=> opt::none) == std::strong_ordering::equal);
            check((o2 <=> opt::none) == std::strong_ordering::greater);
            check((o3 <=> opt::none) == std::strong_ordering::greater);
        }
        { // operator==(const option<T1>&, const T2&)
            check(!(o1 == first_value));
            check(o2 == first_value);
            check((o3 == first_value) == (second_value == first_value));
            check(!(o1 == second_value));
            check((o2 == second_value) == (first_value == second_value));
            check(o3 == second_value);
        }
        { // operator==(const T1&, const option<T2>&)
            check(!(first_value == o1));
            check(first_value == o2);
            check((first_value == o3) == (first_value == second_value));
            check(!(second_value == o1));
            check((second_value == o2) == (second_value == first_value));
            check(second_value == o3);
        }
        { // operator!=(const option<T1>&, const T2&)
            check(o1 != first_value);
            check(!(o2 != first_value));
            check((o3 != first_value) == (second_value != first_value));
            check(o1 != second_value);
            check((o2 != second_value) == (first_value != second_value));
            check(!(o3 != second_value));
        }
        { // operator!=(const T1&, const option<T2>&)
            check(first_value != o1);
            check(!(first_value != o2));
            check((first_value != o3) == (first_value != second_value));
            check(second_value != o1);
            check((second_value != o2) == (second_value != first_value));
            check(!(second_value != o3));
        }
        { // operator<(const option<T1>&, const T2&)
            check(o1 < first_value);
            check(!(o2 < first_value));
            check((o3 < first_value) == (second_value < first_value));
            check(o1 < second_value);
            check((o2 < second_value) == (first_value < second_value));
            check(!(o3 < second_value));
        }
        { // operator<(const T1&, const option<T2>&)
            check(!(first_value < o1));
            check(!(first_value < o2));
            check((first_value < o3) == (first_value < second_value));
            check(!(second_value < o1));
            check((second_value < o2) == (second_value < first_value));
            check(!(second_value < o3));
        }
        { // operator<=(const option<T1>&, const T2&)
            check(o1 <= first_value);
            check(o2 <= first_value);
            check((o3 <= first_value) == (second_value <= first_value));
            check(o1 <= second_value);
            check((o2 <= second_value) == (first_value <= second_value));
            check(o3 <= second_value);
        }
        { // operator<=(const T1&, const option<T2>&)
            check(!(first_value <= o1));
            check(first_value <= o2);
            check((first_value <= o3) == (first_value <= second_value));
            check(!(second_value <= o1));
            check((second_value <= o2) == (second_value <= first_value));
            check(second_value <= o3);
        }
        { // operator>(const option<T1>&, const T2&)
            check(!(o1 > first_value));
            check(!(o2 > first_value));
            check((o3 > first_value) == (second_value > first_value));
            check(!(o1 > second_value));
            check((o2 > second_value) == (first_value > second_value));
            check(!(o3 > second_value));
        }
        { // operator>(const T1&, const option<T2>&)
            check(first_value > o1);
            check(!(first_value > o2));
            check((first_value > o3) == (first_value > second_value));
            check(second_value > o1);
            check((second_value > o2) == (second_value > first_value));
            check(!(second_value > o3));
        }
        { // operator>=(const option<T1>&, const T2&)
            check(!(o1 >= first_value));
            check(o2 >= first_value);
            check((o3 >= first_value) == (second_value >= first_value));
            check(!(o1 >= second_value));
            check((o2 >= second_value) == (first_value >= second_value));
            check(o3 >= second_value);
        }
        { // operator>=(const T1&, const option<T2>&)
            check(first_value >= o1);
            check(first_value >= o2);
            check((first_value >= o3) == (first_value >= second_value));
            check(second_value >= o1);
            check((second_value >= o2) == (second_value >= first_value));
            check(second_value >= o3);
        }
        // operator<=>(const option<T1>&, const T2&)
        if constexpr (std::three_way_comparable_with<T, std::remove_cv_t<T>>) {
            check((o1 <=> first_value) == std::strong_ordering::less);
            check((o2 <=> first_value) == std::strong_ordering::equal);
            check((o3 <=> first_value) == (second_value <=> first_value));
            check((o1 <=> second_value) == std::strong_ordering::less);
            check((o2 <=> second_value) == (first_value <=> second_value));
            check((o3 <=> second_value) == std::strong_ordering::equal);
        }
    }
}

template<typename T>
void test_generic_opt_option(const std::remove_cv_t<T>& first_value, const std::remove_cv_t<T>& second_value) {
    { // operator=
        { // operator=(none_t)
            {
                opt::option<T> o{first_value};
                check(o.has_value());
                o = opt::none;
                check(!o.has_value());

                static_assert(std::is_same_v<decltype(o = opt::none), opt::option<T>&>);
            }
            {
                opt::option<T> o;
                check(!o.has_value());
                o = opt::none;
                check(!o.has_value());
            }
        }
        { // operator=(U&& value)
            {
                opt::option<T> o{first_value};
                check(o.get() == first_value);
                o = second_value;
                check(o.get() == second_value);
                o = first_value;
                check(o.get() == first_value);

                static_assert(std::is_same_v<decltype(o = first_value), opt::option<T>&>);
            }
            {
                opt::option<T> o;
                check(!o.has_value());
                o = second_value;
                check(o.get() == second_value);
            }
        }
        { // operator=(const option&)
            {
                opt::option<T> o1{first_value};
                check(o1.get() == first_value);
                opt::option<T> o2{second_value};
                check(o2.get() == second_value);

                o1 = o2;
                check(o1.get() == second_value);
                check(o2.get() == second_value);

                static_assert(std::is_same_v<decltype(o1 = o2), opt::option<T>&>);
            }
            {
                opt::option<T> o1;
                check(!o1.has_value());
                opt::option<T> o2{second_value};
                check(o2.get() == second_value);

                o1 = o2;
                check(o1.get() == second_value);
                check(o2.get() == second_value);
            }
            {
                opt::option<T> o1{first_value};
                check(o1.get() == first_value);
                opt::option<T> o2;
                check(!o2.has_value());

                o1 = o2;
                check(!o1.has_value());
                check(!o2.has_value());
            }
            {
                opt::option<T> o1;
                check(!o1.has_value());
                opt::option<T> o2;
                check(!o2.has_value());

                o1 = o2;
                check(!o1.has_value());
                check(!o2.has_value());
            }
            { // self-assignment
                opt::option<T> o;
                check(!o.has_value());

                o = o;
                check(!o.has_value());

                o = first_value;
                check(o.has_value());
                o = o;
                check(o.has_value());
                check(o.get() == first_value);
            }
        }
        { // operator=(option&&)
            {
                opt::option<T> o1{first_value};
                check(o1.get() == first_value);
                opt::option<T> o2{second_value};
                check(o2.get() == second_value);

                o1 = std::move(o2);
                check(o1.get() == second_value);
                check(o2.has_value());

                static_assert(std::is_same_v<decltype(o1 = std::move(o2)), opt::option<T>&>);
            }
            {
                opt::option<T> o1;
                check(!o1.has_value());
                opt::option<T> o2{second_value};
                check(o2.get() == second_value);

                o1 = std::move(o2);
                check(o1.get() == second_value);
                check(o2.has_value());
            }
            {
                opt::option<T> o1{first_value};
                check(o1.get() == first_value);
                opt::option<T> o2;
                check(!o2.has_value());

                o1 = std::move(o2);
                check(!o1.has_value());
                check(!o2.has_value());
            }
            {
                opt::option<T> o1;
                check(!o1.has_value());
                opt::option<T> o2;
                check(!o2.has_value());

                o1 = std::move(o2);
                check(!o1.has_value());
                check(!o2.has_value());
            }
            { // self-assignment
                opt::option<T> o;
                check(!o.has_value());

                o = std::move(o);
                check(!o.has_value());

                o = first_value;
                check(o.has_value());
                o = std::move(o);
                check(o.has_value()); // after self-assignment most T types are in unspecified state
            }
        }
    }
    { // .reset()
        {
            opt::option<T> o{first_value};
            check(o.has_value());
            check(o.get() == first_value);
            o.reset();
            check(!o.has_value());
        }
        {
            opt::option<T> o;
            check(!o.has_value());
            o.reset();
            check(!o.has_value());
        }
    }
    { // emplace(Args&&... args)
        {
            opt::option<T> o{first_value};
            check(o.has_value());

            [[maybe_unused]] auto&& r = o.emplace(second_value);
            check(o.get() == second_value);

            auto& a = o.emplace(second_value);
            check(o.get() == second_value);
            check(&a == &o.get());

            a = first_value;
            check(o.get() == first_value);
            check(a == first_value);

            static_assert(std::is_same_v<decltype(o.emplace(first_value)), T&>);
        }
        {
            opt::option<T> o;
            check(!o.has_value());

            [[maybe_unused]] auto&& _1 = o.emplace(first_value);
            check(o.has_value());
            check(o.get() == first_value);

            [[maybe_unused]] auto&& _2 = o.emplace(first_value);
            check(o.has_value());
            check(o.get() == first_value);

            auto& a = o.emplace(second_value);
            check(o.has_value());
            check(o.get() == second_value);
            check(&a == &o.get());
        }
        {
            opt::option<T> o;
            check(!o.has_value());

            [[maybe_unused]] auto&& _1 = o.emplace(second_value);
            check(o.has_value());
            check(o.get() == second_value);

            [[maybe_unused]] auto&& _2 = o.emplace(first_value);
            check(o.has_value());
            check(o.get() == first_value);
        }
    }
    { // swap(option&)
        {
            opt::option<T> o1{first_value};
            opt::option<T> o2{second_value};
            check(o1.has_value());
            check(o2.has_value());

            o1.swap(o2);
            check(o1.get() == second_value);
            check(o2.get() == first_value);
        }
        {
            opt::option<T> o1;
            opt::option<T> o2{first_value};
            check(!o1.has_value());
            check(o2.has_value());

            o1.swap(o2);
            check(o1.get() == first_value);
            check(!o2.has_value());
        }
        {
            opt::option<T> o1{first_value};
            opt::option<T> o2;
            check(o1.has_value());
            check(!o2.has_value());

            o1.swap(o2);
            check(!o1.has_value());
            check(o2.get() == first_value);
        }
        {
            opt::option<T> o1;
            opt::option<T> o2;
            check(!o1.has_value());
            check(!o2.has_value());

            o1.swap(o2);
            check(!o1.has_value());
            check(!o2.has_value());
        }
    }
    test_address_sanitizer<T>(first_value, second_value);
}

template<typename T>
void test_const_and_non_const_generic_opt_option(const std::remove_cv_t<T>& first_value, const std::remove_cv_t<T>& second_value) {
    test_generic_opt_option<T>(first_value, second_value);
    test_generic_const_opt_option<T>(first_value, second_value);
    test_generic_const_opt_option<const T>(first_value, second_value);
}

template<typename T>
void test_cv_generic_opt_option(const std::remove_cv_t<T>& first_value, const std::remove_cv_t<T>& second_value) {
    test_generic_opt_option<T>(first_value, second_value);
    test_generic_opt_option<volatile T>(first_value, second_value);
    test_generic_const_opt_option<T>(first_value, second_value);
    test_generic_const_opt_option<const T>(first_value, second_value);
    test_generic_const_opt_option<volatile T>(first_value, second_value);
    test_generic_const_opt_option<volatile const T>(first_value, second_value);
}

void test_special_opt_option() {
    { // option(U&& value) ctor
        opt::option<uint32_t> o1{uint8_t{1}};
        check(o1.get() == 1);

        opt::option<int16_t> o2{uint8_t{123}};
        check(o2.get() == 123);

        opt::option o3 = 5u;
        check(o3.get() == 5);
    }
    { // option(const option<U>& other) ctor
        {
            opt::option o1 = 1u;
            opt::option<int64_t> o2{o1};
            check(o2.get() == 1);
            check(o1.get() == 1);
        }
        {
            opt::option<uint16_t> o1;
            opt::option<int32_t> o2{o1};
            check(!o2.has_value());
            check(!o1.has_value());
            o1 = uint16_t{5};
            check(!o2.has_value());
            check(o1.get() == 5);
            o2 = 10;
            check(o2.get() == 10);
            check(o1.get() == 5);
        }
    }
    { // option(option<U>&& other)
        {
            opt::option<std::string> o1 = "123";
            opt::option<std::string_view> o2{std::move(o1)};
            check(o2.get() == "123");
            check(o1.has_value());
        }
        {
            opt::option<std::string> o1;
            opt::option<std::string_view> o2{std::move(o1)};
            check(!o2.has_value());
            check(!o1.has_value());
            o1 = "456";
            check(!o2.has_value());
            check(o1.get() == "456");
            o2 = "789";
            check(o2.get() == "789");
            check(o1.get() == "456");
        }
    }
    { // option(in_place_t, Args&&...)
        opt::option<some_struct> o1{std::in_place, 10, 20.f, 'z'};
        check(o1->a == 10);
        check(o1->b == 20.f);
        check(o1->c == 'z');
    }
    { // option(in_place_t, initializer_list<U>, Args&&...)
        {
            std::initializer_list<int> il = {1, 2, 3};
            opt::option<std::initializer_list<int>> o{std::in_place, il};
            check(o.has_value());
            check(o->size() == 3);
            check(o->begin()[0] == 1);
            check(o->begin()[1] == 2);
            check(o->begin()[2] == 3);
        }
        {
            opt::option<std::vector<int>> o{std::in_place, {4, 5, 6, 7}};
            check(o.has_value());
            check(o->size() == 4);
            check(o.get()[0] == 4);
            check(o.get()[1] == 5);
            check(o.get()[2] == 6);
            check(o.get()[3] == 7);
        }
    }
    { // operator=(const option<U>& other)
        {
            opt::option<std::string> o1 = "abc";
            check(o1.get() == "abc");
            opt::option<std::string_view> o2 = "def";
            check(o2.get() == "def");

            o1 = o2;
            check(o1.get() == "def");
            check(o2.get() == "def");
        }
        {
            opt::option<std::string> o1;
            check(!o1.has_value());
            opt::option<std::string_view> o2 = "abc";
            check(o2.get() == "abc");

            o1 = o2;
            check(o1.get() == "abc");
            check(o2.get() == "abc");
        }
        {
            opt::option<std::string> o1 = "abc";
            check(o1.get() == "abc");
            opt::option<std::string_view> o2;
            check(!o2.has_value());

            o1 = o2;
            check(!o1.has_value());
            check(!o2.has_value());
        }
        {
            opt::option<std::string> o1;
            check(!o1.has_value());
            opt::option<std::string_view> o2;
            check(!o2.has_value());

            o1 = o2;
            check(!o1.has_value());
            check(!o2.has_value());
        }
    }
    { // operator=(option<U>&& other)
        {
            opt::option<std::string_view> o1 = "abc";
            check(o1.get() == "abc");
            opt::option<std::string> o2 = "def";
            check(o2.get() == "def");

            o1 = std::move(o2);
            check(o1.get() == "def");
            check(o2.has_value());
        }
        {
            opt::option<std::string_view> o1;
            check(!o1.has_value());
            opt::option<std::string> o2 = "abc";
            check(o2.get() == "abc");

            o1 = std::move(o2);
            check(o1.get() == "abc");
            check(o2.has_value());
        }
        {
            opt::option<std::string_view> o1 = "abc";
            check(o1.get() == "abc");
            opt::option<std::string> o2;
            check(!o2.has_value());

            o1 = std::move(o2);
            check(!o1.has_value());
            check(!o2.has_value());
        }
        {
            opt::option<std::string_view> o1;
            check(!o1.has_value());
            opt::option<std::string> o2;
            check(!o2.has_value());

            o1 = std::move(o2);
            check(!o1.has_value());
            check(!o2.has_value());
        }
    }
    { // emplace(std::initializer_list<U>, Args&&...)
        {
            std::initializer_list<int> il1 = {1, 2, 3};
            opt::option o{il1};
            check(o.has_value());
            check(o->size() == 3);
            check(o->begin()[0] == 1);
            check(o->begin()[1] == 2);
            check(o->begin()[2] == 3);

            std::initializer_list<int> il2 = {4, 5};
            auto& a = o.emplace(il2);
            check(o.has_value());
            check(o->size() == 2);
            check(o->begin()[0] == 4);
            check(o->begin()[1] == 5);
            check(&a == &o.get());

            o.reset();
            check(!o.has_value());

            std::initializer_list<int> il3 = {10};
            o.emplace(il3);
            check(o.has_value());
            check(o->size() == 1);
            check(o->begin()[0] == 10);
        }
        {
            opt::option<std::vector<int>> o;
            check(!o.has_value());

            o.emplace({-1, -2, -3});
            check(o.has_value());
            check(o->size() == 3);
            check(o.get()[0] == -1);
            check(o.get()[1] == -2);
            check(o.get()[2] == -3);

            o.reset();
            check(!o.has_value());

            o.emplace({0});
            check(o.has_value());
            check(o->size() == 1);
            check(o->front() == 0);
        }
    }
}

struct base {
    base(int base_value_) : base_value{base_value_} {}
    virtual ~base() = default;

    int base_value;
};

struct derived : base {
    derived(int base_value_, int derived_value_) : base{base_value_}, derived_value{derived_value_} {}

    int derived_value;
};

void test_unique_ptr_opt_option() {
    { // option()
        opt::option<std::unique_ptr<int>> o;
        check(!o.has_value());
    }
    { // option(none_t)
        opt::option<std::unique_ptr<int>> o{opt::none};
        check(!o.has_value());
    }
    { // option(U&& value)
        {
            opt::option<std::unique_ptr<int>> o{std::make_unique<int>(123)};
            check(*o.get() == 123);
        }
        {
            opt::option<std::unique_ptr<base>> o{std::make_unique<derived>(1, 2)};
            check(o.get()->base_value == 1);
        }
    }
    { // option(option&&)
        opt::option<std::unique_ptr<int>> o1{std::make_unique<int>(3)};
        check(o1.has_value());

        opt::option<std::unique_ptr<int>> o2{std::move(o1)};
        check(*o2.get() == 3);
    }
    { // option(option<U>&& other)
        opt::option<std::unique_ptr<derived>> o1{std::make_unique<derived>(1, 2)};
        check(o1.get()->base_value == 1);
        check(o1.get()->derived_value == 2);

        opt::option<std::unique_ptr<base>> o2{std::move(o1)};
        check(o2.get()->base_value == 1);
    }
    { // option(in_place_t, Args&&...)
        opt::option<std::unique_ptr<int>> o{std::in_place, std::make_unique<int>(1)};
        check(*o.get() == 1);
    }
    { // operator=(U&&)
        {
            opt::option<std::unique_ptr<int>> o;
            check(!o.has_value());

            o = std::make_unique<int>(5);
            check(o.has_value());
            check(*o.get() == 5);
        }
        {
            opt::option<std::unique_ptr<int>> o{std::make_unique<int>(10)};
            check(o.has_value());
            check(*o.get() == 10);

            o = std::make_unique<int>(5);
            check(o.has_value());
            check(*o.get() == 5);
        }
    }
    { // operator=(option<U>&&)
        {
            opt::option<std::unique_ptr<base>> o1;
            check(!o1.has_value());

            opt::option<std::unique_ptr<derived>> o2 = std::make_unique<derived>(-1, -2);
            check(o2.has_value());

            o1 = std::move(o2);
            check(o1.has_value());
            check(o1.get()->base_value == -1);
            check(o2.has_value());
        }
    }
    { // reset()
        opt::option<std::unique_ptr<int>> o;
        check(!o.has_value());

        o.reset();
        check(!o.has_value());

        o = std::make_unique<int>(3);
        check(*o.get() == 3);

        o.reset();
        check(!o.has_value());
    }
    { // emplace(Args&&...)
        {
            opt::option<std::unique_ptr<int>> o;
            check(!o.has_value());

            o.emplace(std::make_unique<int>(23));
            check(o.has_value());
            check(*o.get() == 23);

            o.emplace(std::make_unique<int>(32));
            check(o.has_value());
            check(*o.get() == 32);
        }
        {
            opt::option<std::unique_ptr<base>> o;
            check(!o.has_value());

            o.emplace(std::make_unique<derived>(20, 30));
            check(o.has_value());
            check(o.get()->base_value == 20);

            o.emplace(std::make_unique<base>(40));
            check(o.has_value());
            check(o.get()->base_value == 40);
        }
    }
}
}

void main_test_suite() {
    test_cv_generic_opt_option<int>(1, 2);
    test_cv_generic_opt_option<bool>(true, false);
    test_cv_generic_opt_option<float>(1.f, 2.f);
    test_cv_generic_opt_option<double>(2., 3.);
    test_const_and_non_const_generic_opt_option<some_struct>(some_struct{1, 2.f, 'a'}, some_struct{2, 3.f, 'b'});
    test_const_and_non_const_generic_opt_option<std::string>("abcdefg", "supersupermegalongstringthatwillnotbesso");
    test_const_and_non_const_generic_opt_option<std::vector<int>>(std::vector{1, 2, 3, 4, 5}, std::vector{6, 7, 8, 9});
    test_const_and_non_const_generic_opt_option<std::vector<std::string>>(std::vector<std::string>{"hello", "world"}, std::vector<std::string>{"the string"});

    test_special_opt_option();
    test_unique_ptr_opt_option();
}

OPTION_SUPPRESS_COMMON_WARNINGS_POP
