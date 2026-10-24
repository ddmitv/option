
#include <tests.hpp>
#include <opt/option.hpp>
#include <ranges>
#include <type_traits>
#include <utility>

namespace {

template<typename T>
constexpr void test_begin() {
    {
        T x = T{};
        opt::option<T> o{x};
        auto it1 = o.begin();
        auto it2 = std::as_const(o).begin();
        check(*it1 == it1[0]);
        check(*it2 == it2[0]);
        check(it1 == o.begin());
        check(it2 == std::as_const(o).begin());

        static_assert(noexcept(o.begin()));
        static_assert(noexcept(std::as_const(o).begin()));
    }
    {
        opt::option<T> o = opt::none;
        auto it1 = o.begin();
        auto it2 = std::as_const(o).begin();
        check(it1 == o.begin());
        check(it2 == std::as_const(o).begin());
    }
}

constexpr bool run_begin_tests() {
    test_begin<int>();
    test_begin<char>();
    test_begin<const int>();
    test_begin<const char>();
    // test_begin<int&>();
    // test_begin<char&>();
    // test_begin<const int&>();
    // test_begin<const char&>();
    return true;
}

template<typename T>
constexpr void test_end() {
    {
        opt::option<T> o = opt::none;

        auto it1 = o.end();
        auto it2 = std::as_const(o).end();

        check(it1 == o.begin());
        check(o.begin() == it1);
        check(it1 == o.end());

        check(it2 == std::as_const(o).begin());
        check(std::as_const(o).begin() == it2);
        check(it2 == std::as_const(o).end());

        static_assert(noexcept(o.end()));
        static_assert(noexcept(std::as_const(o).end()));
    }
    {
        T x = T{};
        opt::option<T> o{x};

        auto it1 = o.end();
        auto it2 = std::as_const(o).end();

        check(it1 != o.begin());
        check(o.begin() != it1);

        check(it2 != std::as_const(o).begin());
        check(std::as_const(o).begin() != it2);
    }
}

constexpr bool run_end_tests() {
    test_end<int>();
    test_end<char>();
    test_end<const int>();
    test_end<const char>();
    // test_end<int&>();
    // test_end<char&>();
    // test_end<const int&>();
    // test_end<const char&>();
    return true;
}

template<typename T>
constexpr void test_cmp() {
    static_assert(std::three_way_comparable<typename opt::option<T>::iterator>);
    static_assert(std::three_way_comparable<typename opt::option<T>::const_iterator>);

    T x{};
    opt::option<T> o{x};
    {
        auto it1 = o.begin();
        auto it2 = o.begin();
        check(it1 == it2);
        check(!(it1 != it2));

        check_meta(it1 <=> it2, std::strong_ordering, std::strong_ordering::equal, noexcept);

        auto it3 = o.end();
        check(it1 != it3);
        check(it1 <= it3);
        check(it1 < it3);
        check(it3 >= it1);
        check(it3 > it1);

        check(it1 <=> it3 == std::strong_ordering::less);
        check(it3 <=> it1 == std::strong_ordering::greater);
    }
    {
        auto it1 = std::as_const(o).begin();
        auto it2 = std::as_const(o).begin();
        check(it1 == it2);
        check(!(it1 != it2));

        check_meta(it1 <=> it2, std::strong_ordering, std::strong_ordering::equal, noexcept);

        auto it3 = std::as_const(o).end();
        check(it1 != it3);
        check(it1 <= it3);
        check(it1 < it3);
        check(it3 >= it1);
        check(it3 > it1);

        check(it1 <=> it3 == std::strong_ordering::less);
        check(it3 <=> it1 == std::strong_ordering::greater);
    }
}

constexpr bool run_cmp_tests() {
    test_cmp<int>();
    test_cmp<char>();
    // test_cmp<int&>();
    return true;
}

template<typename T>
concept has_iterator = requires { typename T::iterator; };
template<typename T>
concept has_const_iterator = requires { typename T::const_iterator; };

template<typename T>
concept has_both_iterators = has_iterator<T> && has_const_iterator<T>;

template<typename T>
concept only_has_iterator = has_iterator<T> && !has_const_iterator<T>;

template<typename T>
concept only_no_iterator = !has_iterator<T> && !has_const_iterator<T>;

template<typename T>
constexpr void test_iterator(std::decay_t<T> v) {
    {
        opt::option<T> o{v};
        static_assert(std::ranges::range<decltype(o)>);

        auto it1 = o.begin();
        auto it2 = std::as_const(o).begin();
        check(*it1 == o.get());
        check(*it2 == std::as_const(o).get());

        check(std::ranges::size(o) == 1);
        check(std::ranges::size(std::as_const(o)) == 1);

        static_assert(std::contiguous_iterator<decltype(it1)>);
        static_assert(std::contiguous_iterator<decltype(it2)>);

        static_assert(std::random_access_iterator<decltype(it1)>);
        static_assert(std::random_access_iterator<decltype(it2)>);

        static_assert(std::is_same_v<typename decltype(it1)::value_type, std::remove_cvref_t<T>>);
        static_assert(std::is_same_v<typename decltype(it1)::reference, std::remove_reference_t<T>&>);
        static_assert(std::is_same_v<typename decltype(it2)::value_type, std::remove_cvref_t<T>>);
        static_assert(std::is_same_v<typename decltype(it2)::reference, const std::remove_reference_t<T>&>);
    }
    {
        const opt::option<T> o1{opt::none};
        opt::option<T> o2{opt::none};

        check(std::ranges::size(o1) == 0);
        check(std::ranges::size(o2) == 0);
    }
    {
        static_assert(std::ranges::enable_view<opt::option<T>> == true);
        // static_assert(std::format_kind<opt::option<T>> == true);
    }
    {
        opt::option<T> o{v};
        check(o.begin() != o.end());
        o.reset();
        check(o.begin() == o.end());
        o.emplace(v);
        check(o.begin() != o.end());
        check(*(o.begin()) == v);
    }
    {
        opt::option<T> o{v};
        auto it1 = o.begin();
        auto it2 = o.begin();
        auto it3 = o.end();

        auto cit1 = std::as_const(o).begin();
        auto cit2 = std::as_const(o).begin();
        auto cit3 = std::as_const(o).end();

        check(it1 - it2 == 0);
        check(cit1 - cit2 == 0);
        check(it1 - cit1 == 0);
        check(it3 - it1 == 1);
        check(it1 - it3 == -1);

        check(cit3 - cit1 == 1);
        check(cit1 - cit3 == -1);
        check(cit3 - cit3 == 0);
        check(cit3 - it1 == 1);
        check(it1 - cit3 == -1);
    }
}

constexpr bool run_iterator_tests() {
    static_assert(has_both_iterators<opt::option<int>>);
    static_assert(has_both_iterators<opt::option<const int>>);

    test_iterator<int>(1);
    test_iterator<char>('a');
    test_iterator<bool>(true);
    test_iterator<const int>(2);
    test_iterator<const char>('b');
    // test_iterator<int&>(1);
    // test_iterator<char&>('a');
    // test_iterator<bool&>(true);
    // test_iterator<const int&>(2);
    // test_iterator<const char&>('b');

    return true;
}

template<typename T>
constexpr void test_value_init() {
    {
        typename opt::option<T>::iterator it1{};
        typename opt::option<T>::iterator it2{};

        check(it1 == it2);
        check(!(it1 != it2));
        check(it1 <= it2);
        check(it1 >= it2);
        check(!(it1 < it2));
        check(!(it1 > it2));

        check_meta(it1 <=> it2, std::strong_ordering, std::strong_ordering::equal, noexcept);
        check(it1 - it2 == 0);
    }
    {
        typename opt::option<T>::const_iterator it1{};
        typename opt::option<T>::const_iterator it2{};

        check(it1 == it2);
        check(!(it1 != it2));
        check(it1 <= it2);
        check(it1 >= it2);
        check(!(it1 < it2));
        check(!(it1 > it2));

        check_meta(it1 <=> it2, std::strong_ordering, std::strong_ordering::equal, noexcept);
        check(it1 - it2 == 0);
    }
}

constexpr bool run_value_init_tests() {
    test_value_init<int>();
    test_value_init<char>();
    test_value_init<long long>();
    test_value_init<const int>();
    test_value_init<const char>();
    test_value_init<const long long>();
    return true;
}

template<typename T>
constexpr void test_increment_decrement() {
    opt::option<T> o{T{}};

    auto it = o.begin();
    auto& r1 = ++it;
    check(&r1 == &it);
    check(it == o.end());

    auto old = it--;
    check(old == o.end());
    check(it == o.begin());

    auto prev = it++;
    check(prev == o.begin());
    check(it == o.end());

    auto& r2 = --it;
    check(&r2 == &it);
    check(it == o.begin());

    auto cit = std::as_const(o).begin();
    ++cit;
    check(cit == std::as_const(o).end());
    --cit;
    check(cit == std::as_const(o).begin());
}

constexpr bool run_increment_decrement_tests() {
    test_increment_decrement<int>();
    test_increment_decrement<char>();
    test_increment_decrement<const int>();
    test_increment_decrement<const char>();
    return true;
}

template<typename T>
constexpr void test_arithmetic() {
    opt::option<T> o{T{}};

    auto b = o.begin();
    auto e = o.end();

    check(b + 1 == e);
    check(1 + b == e);
    check(e + 0 == e);
    check(e + (-1) == b);

    check(e - 1 == b);
    check(b - 0 == b);
    check(b - (-1) == e);
    check(b - (-1) == b + 1);

    check(b == b);
    check(b - b == 0);
    check(e == e);
    check(e - e == 0);
    check(e - b == 1);
    check(b - e == -1);

    auto it1 = b;
    auto& r = (it1 += 1);
    check(&r == &it1);
    check(it1 == e);
    it1 -= 1;
    check(it1 == b);

    check(b[0] == *b);
    check(&b[0] == &*b);

    auto it2 = b;
    ++it2;
    check(it2 == b + 1);
    --it2;
    check(it2 == b);
}

constexpr bool run_arithmetic_tests() {
    test_arithmetic<int>();
    test_arithmetic<char>();
    test_arithmetic<const int>();
    test_arithmetic<const char>();
    return true;
}

template<typename T>
constexpr void test_conversions() {
    opt::option<T> o{T{}};

    typename opt::option<T>::iterator it = o.begin();
    typename opt::option<T>::const_iterator cit = it;
    check(cit == it);
    check(it == cit);
    check(cit <=> it == std::strong_ordering::equal);

    static_assert(std::convertible_to<decltype(it), decltype(cit)>);
    static_assert(!std::convertible_to<decltype(cit), decltype(it)>);

    static_assert(!std::constructible_from<decltype(it), decltype(cit)>);
    static_assert(std::constructible_from<decltype(cit), decltype(it)>);
}

constexpr bool run_conversions_tests() {
    test_conversions<int>();
    test_conversions<unsigned int>();
    test_conversions<char>();
    return true;
}

}

void option_iterator_test_suite() {
    static_assert(run_begin_tests());
    run_begin_tests();

    static_assert(run_end_tests());
    run_end_tests();

    static_assert(run_cmp_tests());
    run_cmp_tests();

    static_assert(run_iterator_tests());
    run_iterator_tests();

    static_assert(run_value_init_tests());
    run_value_init_tests();

    static_assert(run_increment_decrement_tests());
    run_increment_decrement_tests();

    static_assert(run_arithmetic_tests());
    run_arithmetic_tests();

    static_assert(run_conversions_tests());
    run_conversions_tests();
}
