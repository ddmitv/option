
#include <opt/option.hpp>
#include <type_traits>

#include <tests.hpp>
#include <comp_ops/comparison_helpers.hpp>

namespace {

namespace equal {
    struct tester {};

    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator==(const tester& a, const T& t) {
        return t.compare(a) == 0;
    }
    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator==(const T& t, const tester& a) {
        return t.compare(a) == 0;
    }

    template<typename T, typename U>
    constexpr opt::option<bool> try_cmp_eq(const T& t, const U& u) {
        // the function is written like because of -Wconsumed
        opt::option<bool> result;
        if constexpr (has_equal<const T, const U>) {
            result = t == u;
        }
        return result;
    }

    static_assert(has_equal<int, opt::option<int>>);
    static_assert(has_equal<int, opt::option<int>>);
    static_assert(has_equal<equality_comparable, opt::option<equality_comparable>>);

    static_assert(!has_equal<non_comparable, opt::option<non_comparable>>);
    static_assert(!has_equal<non_comparable, opt::option<equality_comparable>>);

    static_assert(has_equal<opt::option<int>, int>);
    static_assert(has_equal<opt::option<equality_comparable>, int>);
    static_assert(has_equal<opt::option<equality_comparable>, equality_comparable>);

    static_assert(!has_equal<opt::option<non_comparable>, non_comparable>);
    static_assert(!has_equal<opt::option<equality_comparable>, non_comparable>);

    static_assert(try_cmp_eq(opt::option<int>{}, opt::option<tester>{}) == opt::option<bool>{});
    static_assert(try_cmp_eq(opt::option<int>{}, opt::option<int>{}) == opt::option<bool>{true});

    struct X {
        int value;

        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator==(const X& x, const X& y) {
        return x.value == y.value;
    }

    constexpr bool test() {
        {
            const X val{2};
            const opt::option<X> o1;
            const opt::option<X> o2{1};
            const opt::option<X> o3{val};

            check_meta_auto(o1 == X{1}, false, !noexcept);
            check_meta_auto(o2 == X{1}, true, !noexcept);
            check_meta_auto(o3 == X{1}, false, !noexcept);
            check_meta_auto(o3 == X{2}, true, !noexcept);
            check_meta_auto(o3 == val, true, !noexcept);

            check_meta_auto(X{1} == o1, false, !noexcept);
            check_meta_auto(X{1} == o2, true, !noexcept);
            check_meta_auto(X{1} == o3, false, !noexcept);
            check_meta_auto(X{2} == o3, true, !noexcept);
            check_meta_auto(val == o3, true, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 == 42L, true, !noexcept);
            check_meta_auto(101L == o1, false, !noexcept);
        }
        {
            const opt::option<const int> o1{42};
            check_meta_auto(o1 == 42, true, !noexcept);
            check_meta_auto(101 == o1, false, !noexcept);
        }
        { // check optimized codegen path for integers
            const opt::option<int> o1;
            const opt::option<int> o2{1};

            check_meta_auto(o1 == 1, false, !noexcept);
            check_meta_auto(o1 == 2, false, !noexcept);
            check_meta_auto(o2 == 1, true, !noexcept);
            check_meta_auto(o2 == 2, false, !noexcept);
            check_meta_auto(o1 == 1L, false, !noexcept);
            check_meta_auto(o1 == 2L, false, !noexcept);
            check_meta_auto(o2 == 1L, true, !noexcept);
            check_meta_auto(o2 == 2L, false, !noexcept);

            check_meta_auto(1 == o1, false, !noexcept);
            check_meta_auto(2 == o1, false, !noexcept);
            check_meta_auto(1 == o2, true, !noexcept);
            check_meta_auto(2 == o2, false, !noexcept);
            check_meta_auto(1L == o1, false, !noexcept);
            check_meta_auto(2L == o1, false, !noexcept);
            check_meta_auto(1L == o2, true, !noexcept);
            check_meta_auto(2L == o2, false, !noexcept);
        }

        return true;
    }
} // namespace equal

namespace greater {
    struct tester {};

    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator>(const tester& a, const T& t) {
        return t.compare(a) < 0;
    }
    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator>(const T& t, const tester& a) {
        return t.compare(a) > 0;
    }

    template<typename T, typename U>
    constexpr opt::option<bool> try_cmp_gt(const T& t, const U& u) {
        // the function is written like because of -Wconsumed
        opt::option<bool> result;
        if constexpr (has_greater<const T, const U>) {
            result = t > u;
        }
        return result;
    }

    static_assert(has_greater<opt::option<totally_ordered>, int>);
    static_assert(has_greater<opt::option<totally_ordered>, int>);

    static_assert(!has_greater<opt::option<non_comparable>, non_comparable>);
    static_assert(!has_greater<opt::option<totally_ordered>, non_comparable>);
    static_assert(!has_greater<opt::option<non_comparable>, totally_ordered>);

    static_assert(has_greater<int, opt::option<totally_ordered>>);
    static_assert(has_greater<totally_ordered, opt::option<totally_ordered>>);

    static_assert(!has_greater<non_comparable, opt::option<non_comparable>>);
    static_assert(!has_greater<non_comparable, opt::option<totally_ordered>>);
    static_assert(!has_greater<totally_ordered, opt::option<non_comparable>>);

    // LWG4072: avoid ambiguity with optional's own comparison operators
    static_assert(try_cmp_gt(opt::option<int>{}, opt::option<tester>{}) == opt::option<bool>{});
    static_assert(try_cmp_gt(opt::option<int>{}, opt::option<int>{}) == opt::option<bool>{false});

    struct X {
        int value;

        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator>(const X& x, const X& y) {
        return x.value > y.value;
    }

    constexpr bool test() {
        {
            const X val{2};
            const opt::option<X> o1;
            const opt::option<X> o2{1};
            const opt::option<X> o3{val};

            check_meta_auto(o1 > X{1}, false, !noexcept);
            check_meta_auto(o2 > X{1}, false, !noexcept);
            check_meta_auto(o3 > X{1}, true, !noexcept);
            check_meta_auto(o2 > val, false, !noexcept);
            check_meta_auto(o3 > val, false, !noexcept);
            check_meta_auto(o3 > X{3}, false, !noexcept);

            check_meta_auto(X{1} > o1, true, !noexcept);
            check_meta_auto(X{1} > o2, false, !noexcept);
            check_meta_auto(X{1} > o3, false, !noexcept);
            check_meta_auto(val > o2, true, !noexcept);
            check_meta_auto(val > o3, false, !noexcept);
            check_meta_auto(X{3} > o3, true, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 > 11L, true, !noexcept);
            check_meta_auto(42L > o1, false, !noexcept);
        }
        {
            const opt::option<const int> o1{42};
            check_meta_auto(o1 > 11, true, !noexcept);
            check_meta_auto(42 > o1, false, !noexcept);
        }

        return true;
    }
} // namespace greater

namespace greater_equal {
    struct tester {};

    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator>=(const tester& a, const T& t) {
        return t.compare(a) <= 0;
    }
    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator>=(const T& t, const tester& a) {
        return t.compare(a) >= 0;
    }

    template<typename T, typename U>
    constexpr opt::option<bool> try_cmp_gteq(const T& t, const U& u) {
        // the function is written like because of -Wconsumed
        opt::option<bool> result;
        if constexpr (has_greater_equal<const T, const U>) {
            result = t >= u;
        }
        return result;
    }

    static_assert(has_greater_equal<int, opt::option<int>>);
    static_assert(has_greater_equal<totally_ordered, opt::option<totally_ordered>>);

    static_assert(!has_greater_equal<non_comparable, opt::option<non_comparable>>);
    static_assert(!has_greater_equal<non_comparable, opt::option<totally_ordered>>);
    static_assert(!has_greater_equal<totally_ordered, opt::option<non_comparable>>);

    static_assert(has_greater_equal<opt::option<int>, int>);
    static_assert(has_greater_equal<opt::option<totally_ordered>, totally_ordered>);

    static_assert(!has_greater_equal<opt::option<non_comparable>, non_comparable>);
    static_assert(!has_greater_equal<opt::option<totally_ordered>, non_comparable>);
    static_assert(!has_greater_equal<opt::option<non_comparable>, totally_ordered>);

    // LWG4072: avoid ambiguity with optional's own comparison operators
    static_assert(try_cmp_gteq(opt::option<int>{}, opt::option<tester>{}) == opt::option<bool>{});
    static_assert(try_cmp_gteq(opt::option<int>{}, opt::option<int>{}) == opt::option<bool>{true});

    struct X {
        int value;

        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator>=(const X& x, const X& y) {
        return x.value >= y.value;
    }

    constexpr bool test() {
        {
            const X val{2};
            const opt::option<X> o1;
            const opt::option<X> o2{1};
            const opt::option<X> o3{val};

            check_meta_auto(o1 >= X{1}, false, !noexcept);
            check_meta_auto(o2 >= X{1}, true, !noexcept);
            check_meta_auto(o3 >= X{1}, true, !noexcept);
            check_meta_auto(o2 >= val, false, !noexcept);
            check_meta_auto(o3 >= val, true, !noexcept);
            check_meta_auto(o3 >= X{3}, false, !noexcept);

            check_meta_auto(X{1} >= o1, true, !noexcept);
            check_meta_auto(X{1} >= o2, true, !noexcept);
            check_meta_auto(X{1} >= o3, false, !noexcept);
            check_meta_auto(val >= o2, true, !noexcept);
            check_meta_auto(val >= o3, true, !noexcept);
            check_meta_auto(X{3} >= o3, true, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 >= 42L, true, !noexcept);
            check_meta_auto(11L >= o1, false, !noexcept);
        }
        {
            const opt::option<const int> o1{42};
            check_meta_auto(o1 >= 42, true, !noexcept);
            check_meta_auto(11 >= o1, false, !noexcept);
        }

        return true;
    }
} // namespace greater_equal

namespace less {
    struct tester {};

    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator<(const tester& a, const T& t) {
        return t.compare(a) > 0;
    }
    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator<(const T& t, const tester& a) {
        return t.compare(a) < 0;
    }

    template<typename T, typename U>
    constexpr opt::option<bool> try_cmp_lt(const T& t, const U& u) {
        // the function is written like because of -Wconsumed
        opt::option<bool> result;
        if constexpr (has_less<const T, const U>) {
            result = t < u;
        }
        return result;
    }

    static_assert(has_less<int, opt::option<int>>);
    static_assert(has_less<totally_ordered, opt::option<totally_ordered>>);

    static_assert(!has_less<non_comparable, opt::option<non_comparable>>);
    static_assert(!has_less<non_comparable, opt::option<totally_ordered>>);
    static_assert(!has_less<totally_ordered, opt::option<non_comparable>>);

    static_assert(has_less<opt::option<int>, int>);
    static_assert(has_less<opt::option<totally_ordered>, totally_ordered>);

    static_assert(!has_less<opt::option<non_comparable>, non_comparable>);
    static_assert(!has_less<opt::option<totally_ordered>, non_comparable>);
    static_assert(!has_less<opt::option<non_comparable>, totally_ordered>);

    // LWG4072: avoid ambiguity with optional's own comparison operators
    static_assert(try_cmp_lt(opt::option<int>{}, opt::option<tester>{}) == opt::option<bool>{});
    static_assert(try_cmp_lt(opt::option<int>{}, opt::option<int>{}) == opt::option<bool>{false});

    struct X {
        int value;

        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator<(const X& x, const X& y) {
        return x.value < y.value;
    }

    constexpr bool test() {
        {
            const X val{2};
            const opt::option<X> o1;
            const opt::option<X> o2{1};
            const opt::option<X> o3{val};

            check_meta_auto(o1 < X{1}, true, !noexcept);
            check_meta_auto(o2 < X{1}, false, !noexcept);
            check_meta_auto(o3 < X{1}, false, !noexcept);
            check_meta_auto(o2 < val, true, !noexcept);
            check_meta_auto(o3 < val, false, !noexcept);
            check_meta_auto(o3 < X{3}, true, !noexcept);

            check_meta_auto(X{1} < o1, false, !noexcept);
            check_meta_auto(X{1} < o2, false, !noexcept);
            check_meta_auto(X{1} < o3, true, !noexcept);
            check_meta_auto(val < o2, false, !noexcept);
            check_meta_auto(val < o3, false, !noexcept);
            check_meta_auto(X{3} < o3, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 < 101L, true, !noexcept);
            check_meta_auto(42L < o1, false, !noexcept);
        }
        {
            const opt::option<const int> o1{42};
            check_meta_auto(o1 < 101, true, !noexcept);
            check_meta_auto(42 < o1, false, !noexcept);
        }

        return true;
    }
} // namespace less

namespace less_equal {
    struct tester {};

    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator<=(const tester& a, const T& t) {
        return t.compare(a) >= 0;
    }
    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator<=(const T& t, const tester& a) {
        return t.compare(a) <= 0;
    }

    template<typename T, typename U>
    constexpr opt::option<bool> try_cmp_lteq(const T& t, const U& u) {
        // the function is written like because of -Wconsumed
        opt::option<bool> result;
        if constexpr (has_less_equal<const T, const U>) {
            result = t <= u;
        }
        return result;
    }

    static_assert(has_less_equal<int, opt::option<int>>);
    static_assert(has_less_equal<totally_ordered, opt::option<totally_ordered>>);

    static_assert(!has_less_equal<non_comparable, opt::option<non_comparable>>);
    static_assert(!has_less_equal<non_comparable, opt::option<totally_ordered>>);
    static_assert(!has_less_equal<totally_ordered, opt::option<non_comparable>>);

    static_assert(has_less_equal<opt::option<int>, int>);
    static_assert(has_less_equal<opt::option<totally_ordered>, totally_ordered>);

    static_assert(!has_less_equal<opt::option<non_comparable>, non_comparable>);
    static_assert(!has_less_equal<opt::option<totally_ordered>, non_comparable>);
    static_assert(!has_less_equal<opt::option<non_comparable>, totally_ordered>);

    // LWG4072: avoid ambiguity with optional's own comparison operators
    static_assert(try_cmp_lteq(opt::option<int>{}, opt::option<tester>{}) == opt::option<bool>{});
    static_assert(try_cmp_lteq(opt::option<int>{}, opt::option<int>{}) == opt::option<bool>{true});

    struct X {
        int value;

        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator<=(const X& x, const X& y) {
        return x.value <= y.value;
    }

    constexpr bool test() {
        {
            const X val{2};
            const opt::option<X> o1;
            const opt::option<X> o2{1};
            const opt::option<X> o3{val};

            check_meta_auto(o1 <= X{1}, true, !noexcept);
            check_meta_auto(o2 <= X{1}, true, !noexcept);
            check_meta_auto(o3 <= X{1}, false, !noexcept);
            check_meta_auto(o2 <= val, true, !noexcept);
            check_meta_auto(o3 <= val, true, !noexcept);
            check_meta_auto(o3 <= X{3}, true, !noexcept);

            check_meta_auto(X{1} <= o1, false, !noexcept);
            check_meta_auto(X{1} <= o2, true, !noexcept);
            check_meta_auto(X{1} <= o3, true, !noexcept);
            check_meta_auto(val <= o2, false, !noexcept);
            check_meta_auto(val <= o3, true, !noexcept);
            check_meta_auto(X{3} <= o3, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 <= 42L, true, !noexcept);
            check_meta_auto(101L <= o1, false, !noexcept);
        }
        {
            const opt::option<const int> o1{42};
            check_meta_auto(o1 <= 42, true, !noexcept);
            check_meta_auto(101 <= o1, false, !noexcept);
        }

        return true;
    }
} // namespace less_equal

namespace not_equal {
    struct tester {};

    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator!=(const tester& a, const T& t) {
        return t.compare(a) != 0;
    }
    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr bool operator!=(const T& t, const tester& a) {
        return t.compare(a) != 0;
    }

    template<typename T, typename U>
    constexpr opt::option<bool> try_cmp_neq(const T& t, const U& u) {
        // the function is written like because of -Wconsumed
        opt::option<bool> result;
        if constexpr (has_not_equal<const T, const U>) {
            result = t != u;
        }
        return result;
    }

    static_assert(has_not_equal<int, opt::option<int>>);
    static_assert(has_not_equal<totally_ordered, opt::option<totally_ordered>>);

    static_assert(!has_not_equal<non_comparable, opt::option<non_comparable>>);
    static_assert(!has_not_equal<non_comparable, opt::option<totally_ordered>>);
    static_assert(!has_not_equal<totally_ordered, opt::option<non_comparable>>);

    static_assert(has_not_equal<opt::option<int>, int>);
    static_assert(has_not_equal<opt::option<totally_ordered>, totally_ordered>);

    static_assert(!has_not_equal<opt::option<non_comparable>, non_comparable>);
    static_assert(!has_not_equal<opt::option<totally_ordered>, non_comparable>);
    static_assert(!has_not_equal<opt::option<non_comparable>, totally_ordered>);

    // LWG4072: avoid ambiguity with optional's own comparison operators
    static_assert(try_cmp_neq(opt::option<int>{}, opt::option<tester>{}) == opt::option<bool>{});
    static_assert(try_cmp_neq(opt::option<int>{}, opt::option<int>{}) == opt::option<bool>{false});

    struct X {
        int value;

        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator!=(const X& x, const X& y) {
        return x.value != y.value;
    }

    constexpr bool test() {
        {
            const X val{2};
            const opt::option<X> o1;
            const opt::option<X> o2{1};
            const opt::option<X> o3{val};

            check_meta_auto(o1 != X{1}, true, !noexcept);
            check_meta_auto(o2 != X{1}, false, !noexcept);
            check_meta_auto(o3 != X{1}, true, !noexcept);
            check_meta_auto(o3 != X{2}, false, !noexcept);
            check_meta_auto(o3 != val, false, !noexcept);

            check_meta_auto(X{1} != o1, true, !noexcept);
            check_meta_auto(X{1} != o2, false, !noexcept);
            check_meta_auto(X{1} != o3, true, !noexcept);
            check_meta_auto(X{2} != o3, false, !noexcept);
            check_meta_auto(val != o3, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 != 42L, false, !noexcept);
            check_meta_auto(101L != o1, true, !noexcept);
        }
        {
            const opt::option<const int> o1{42};
            check_meta_auto(o1 != 42, false, !noexcept);
            check_meta_auto(101 != o1, true, !noexcept);
        }
        {
            const opt::option<int> o1;
            const opt::option<int> o2{1};

            check_meta_auto(o1 != 1, true, !noexcept);
            check_meta_auto(o1 != 2, true, !noexcept);
            check_meta_auto(o2 != 1, false, !noexcept);
            check_meta_auto(o2 != 2, true, !noexcept);
            check_meta_auto(o1 != 1L, true, !noexcept);
            check_meta_auto(o1 != 2L, true, !noexcept);
            check_meta_auto(o2 != 1L, false, !noexcept);
            check_meta_auto(o2 != 2L, true, !noexcept);

            check_meta_auto(1 != o1, true, !noexcept);
            check_meta_auto(2 != o1, true, !noexcept);
            check_meta_auto(1 != o2, false, !noexcept);
            check_meta_auto(2 != o2, true, !noexcept);
            check_meta_auto(1L != o1, true, !noexcept);
            check_meta_auto(2L != o1, true, !noexcept);
            check_meta_auto(1L != o2, false, !noexcept);
            check_meta_auto(2L != o2, true, !noexcept);
        }

        return true;
    }
} // namespace not_equal

namespace three_way {
    struct SomeInt {
        int value;

        constexpr explicit SomeInt(int v = 0) : value{v} {}
        auto operator<=>(const SomeInt&) const = default;
    };

    struct tester {};

    // returns std::strong_ordering because MSVC instantiates the body of the function for auto return type even if it is not a viable candidate (unlike GCC and Clang)
    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr std::strong_ordering operator<=>(const tester& a, const T& t) {
        return t.compare(a) <=> 0;
    }
    template<typename T> requires std::is_class_v<T> // intentionally underconstrained
    [[maybe_unused]] constexpr std::strong_ordering operator<=>(const T& t, const tester& a) {
        return t.compare(a) <=> 0;
    }

    template<typename T, typename U>
    constexpr opt::option<bool> try_cmp_three_way(const T& t, const U& u) {
        // the function is written like because of -Wconsumed
        opt::option<bool> result;
        if constexpr (has_spaceship<const T, const U>) {
            result = (t <=> u) == 0;
        }
        return result;
    }

    static_assert(has_spaceship<SomeInt, opt::option<SomeInt>>);
    static_assert(has_spaceship<opt::option<SomeInt>, SomeInt>);

    static_assert(has_spaceship<int, opt::option<int>>);
    static_assert(has_spaceship<opt::option<int>, int>);

    static_assert(!has_spaceship<non_comparable, opt::option<non_comparable>>);
    static_assert(!has_spaceship<non_comparable, opt::option<SomeInt>>);
    static_assert(!has_spaceship<SomeInt, opt::option<non_comparable>>);

    static_assert(!has_spaceship<opt::option<non_comparable>, non_comparable>);
    static_assert(!has_spaceship<opt::option<non_comparable>, SomeInt>);
    static_assert(!has_spaceship<opt::option<SomeInt>, non_comparable>);

    // LWG4072: avoid ambiguity with optional's own comparison operators
    static_assert(try_cmp_three_way(opt::option<int>{}, opt::option<tester>{}) == opt::option<bool>{});
    static_assert(try_cmp_three_way(opt::option<int>{}, opt::option<int>{}) == opt::option<bool>{true});

    constexpr void test_custom_integral() {
        {
            SomeInt t{3};
            opt::option<SomeInt> op{3};
            check_meta(test_order(t, op, std::strong_ordering::equal), bool, true, !noexcept);
        }
        {
            SomeInt t{2};
            opt::option<SomeInt> op{3};
            check_meta(test_order(t, op, std::strong_ordering::less), bool, true, !noexcept);
        }
        {
            SomeInt t{3};
            opt::option<SomeInt> op{2};
            check_meta(test_order(t, op, std::strong_ordering::greater), bool, true, !noexcept);
        }
    }

    constexpr void test_int() {
        {
            int t{3};
            opt::option<int> op{3};
            check_meta(test_order(t, op, std::strong_ordering::equal), bool, true, !noexcept);
        }
        {
            int t{2};
            opt::option<int> op{3};
            check_meta(test_order(t, op, std::strong_ordering::less), bool, true, !noexcept);
        }
        {
            int t{3};
            opt::option<int> op{2};
            check_meta(test_order(t, op, std::strong_ordering::greater), bool, true, !noexcept);
        }
    }

    constexpr bool test() {
        test_custom_integral();
        test_int();
        return true;
    }
} // namespace three_way

}

void comp_with_t_test_suite() {
    static_assert(equal::test());
    equal::test();

    static_assert(not_equal::test());
    not_equal::test();

    static_assert(greater::test());
    greater::test();

    static_assert(greater_equal::test());
    greater_equal::test();

    static_assert(less::test());
    less::test();

    static_assert(less_equal::test());
    less_equal::test();

    static_assert(three_way::test());
    three_way::test();
}
