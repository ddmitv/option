
#include <opt/option.hpp>
#include <tests.hpp>
#include <comp_ops/comparison_helpers.hpp>

#include <compare>
#include <limits>

namespace {

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.relops/equal.pass.cpp
namespace equal {
    static_assert(has_equal<opt::option<int>>);
    static_assert(has_equal<opt::option<equality_comparable>>);
    static_assert(has_equal<opt::option<equality_comparable>, opt::option<int>>);

    static_assert(!has_equal<opt::option<non_comparable>>);
    static_assert(!has_equal<opt::option<equality_comparable>, opt::option<non_comparable>>);

    struct X {
        int value;
        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator==(const X& x, const X& y) {
        return x.value == y.value;
    }

    template<typename T>
    constexpr void generic_test(const T& val1, const T& val2) {
        const opt::option<T> o1;
        const opt::option<T> o2;
        const opt::option<T> o3{val1};
        const opt::option<T> o4{val2};
        const opt::option<T> o5{val1};

        check_meta_auto(o1 == o1, true, !noexcept);
        check_meta_auto(o1 == o2, true, !noexcept);
        check_meta_auto(o1 == o3, false, !noexcept);
        check_meta_auto(o1 == o4, false, !noexcept);
        check_meta_auto(o1 == o5, false, !noexcept);

        check_meta_auto(o2 == o1, true, !noexcept);
        check_meta_auto(o2 == o2, true, !noexcept);
        check_meta_auto(o2 == o3, false, !noexcept);
        check_meta_auto(o2 == o4, false, !noexcept);
        check_meta_auto(o2 == o5, false, !noexcept);

        check_meta_auto(o3 == o1, false, !noexcept);
        check_meta_auto(o3 == o2, false, !noexcept);
        check_meta_auto(o3 == o3, true, !noexcept);
        check_meta_auto(o3 == o4, false, !noexcept);
        check_meta_auto(o3 == o5, true, !noexcept);

        check_meta_auto(o4 == o1, false, !noexcept);
        check_meta_auto(o4 == o2, false, !noexcept);
        check_meta_auto(o4 == o3, false, !noexcept);
        check_meta_auto(o4 == o4, true, !noexcept);
        check_meta_auto(o4 == o5, false, !noexcept);

        check_meta_auto(o5 == o1, false, !noexcept);
        check_meta_auto(o5 == o2, false, !noexcept);
        check_meta_auto(o5 == o3, true, !noexcept);
        check_meta_auto(o5 == o4, false, !noexcept);
        check_meta_auto(o5 == o5, true, !noexcept);
    }

    constexpr bool test() {
        generic_test<X>(1, 2);
        generic_test<int>(1, 2);
        generic_test<int>(std::numeric_limits<int>::min(), std::numeric_limits<int>::max());
        generic_test<unsigned int>(1u, 2u);
        generic_test<unsigned int>(std::numeric_limits<unsigned int>::min(), std::numeric_limits<unsigned int>::max());
        generic_test<long long>(1ll, 2ll);
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 == opt::option<long>{42}, true, !noexcept);
            check_meta_auto(opt::option<long>{101} == o1, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 == opt::option<const int>{42}, true, !noexcept);
            check_meta_auto(opt::option<const int>{101} == o1, false, !noexcept);
        }

        return true;
    }
} // namespace equal

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.relops/greater_than.pass.cpp
namespace greater {
    static_assert(has_greater<opt::option<totally_ordered>>);
    static_assert(has_greater<opt::option<totally_ordered>, opt::option<int>>);

    static_assert(!has_greater<opt::option<non_comparable>>);
    static_assert(!has_greater<opt::option<equality_comparable>>);
    static_assert(!has_greater<opt::option<totally_ordered>, opt::option<non_comparable>>);

    struct X {
        int value;
        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator>(const X& x, const X& y) {
        return x.value > y.value;
    }

    constexpr bool test() {
        {
            const opt::option<X> o1;
            const opt::option<X> o2;
            const opt::option<X> o3{1};
            const opt::option<X> o4{2};
            const opt::option<X> o5{1};

            check_meta_auto(o1 > o1, false, !noexcept);
            check_meta_auto(o1 > o2, false, !noexcept);
            check_meta_auto(o1 > o3, false, !noexcept);
            check_meta_auto(o1 > o4, false, !noexcept);
            check_meta_auto(o1 > o5, false, !noexcept);

            check_meta_auto(o2 > o1, false, !noexcept);
            check_meta_auto(o2 > o2, false, !noexcept);
            check_meta_auto(o2 > o3, false, !noexcept);
            check_meta_auto(o2 > o4, false, !noexcept);
            check_meta_auto(o2 > o5, false, !noexcept);

            check_meta_auto(o3 > o1, true, !noexcept);
            check_meta_auto(o3 > o2, true, !noexcept);
            check_meta_auto(o3 > o3, false, !noexcept);
            check_meta_auto(o3 > o4, false, !noexcept);
            check_meta_auto(o3 > o5, false, !noexcept);

            check_meta_auto(o4 > o1, true, !noexcept);
            check_meta_auto(o4 > o2, true, !noexcept);
            check_meta_auto(o4 > o3, true, !noexcept);
            check_meta_auto(o4 > o4, false, !noexcept);
            check_meta_auto(o4 > o5, true, !noexcept);

            check_meta_auto(o5 > o1, true, !noexcept);
            check_meta_auto(o5 > o2, true, !noexcept);
            check_meta_auto(o5 > o3, false, !noexcept);
            check_meta_auto(o5 > o4, false, !noexcept);
            check_meta_auto(o5 > o5, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 > opt::option<long>{1}, true, !noexcept);
            check_meta_auto(opt::option<long>{42} > o1, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 > opt::option<const int>{1}, true, !noexcept);
            check_meta_auto(opt::option<const int>{42} > o1, false, !noexcept);
        }

        return true;
    }
} // namespace greater

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.relops/greater_equal.pass.cpp
namespace greater_equal {
    static_assert(has_greater_equal<opt::option<totally_ordered>>);
    static_assert(has_greater_equal<opt::option<totally_ordered>, opt::option<int>>);

    static_assert(!has_greater_equal<opt::option<non_comparable>>);
    static_assert(!has_greater_equal<opt::option<equality_comparable>>);
    static_assert(!has_greater_equal<opt::option<totally_ordered>, opt::option<non_comparable>>);

    struct X {
        int value;
        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator>=(const X& x, const X& y) {
        return x.value >= y.value;
    }

    constexpr bool test() {
        {
            const opt::option<X> o1;
            const opt::option<X> o2;
            const opt::option<X> o3{1};
            const opt::option<X> o4{2};
            const opt::option<X> o5{1};

            check_meta_auto(o1 >= o1, true, !noexcept);
            check_meta_auto(o1 >= o2, true, !noexcept);
            check_meta_auto(o1 >= o3, false, !noexcept);
            check_meta_auto(o1 >= o4, false, !noexcept);
            check_meta_auto(o1 >= o5, false, !noexcept);

            check_meta_auto(o2 >= o1, true, !noexcept);
            check_meta_auto(o2 >= o2, true, !noexcept);
            check_meta_auto(o2 >= o3, false, !noexcept);
            check_meta_auto(o2 >= o4, false, !noexcept);
            check_meta_auto(o2 >= o5, false, !noexcept);

            check_meta_auto(o3 >= o1, true, !noexcept);
            check_meta_auto(o3 >= o2, true, !noexcept);
            check_meta_auto(o3 >= o3, true, !noexcept);
            check_meta_auto(o3 >= o4, false, !noexcept);
            check_meta_auto(o3 >= o5, true, !noexcept);

            check_meta_auto(o4 >= o1, true, !noexcept);
            check_meta_auto(o4 >= o2, true, !noexcept);
            check_meta_auto(o4 >= o3, true, !noexcept);
            check_meta_auto(o4 >= o4, true, !noexcept);
            check_meta_auto(o4 >= o5, true, !noexcept);

            check_meta_auto(o5 >= o1, true, !noexcept);
            check_meta_auto(o5 >= o2, true, !noexcept);
            check_meta_auto(o5 >= o3, true, !noexcept);
            check_meta_auto(o5 >= o4, false, !noexcept);
            check_meta_auto(o5 >= o5, true, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 >= opt::option<long>{42}, true, !noexcept);
            check_meta_auto(opt::option<long>{11} >= o1, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 >= opt::option<const int>{42}, true, !noexcept);
            check_meta_auto(opt::option<const int>{1} >= o1, false, !noexcept);
        }

        return true;
    }
} // namespace greater_equal

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.relops/less_than.pass.cpp
namespace less {
    static_assert(has_less<opt::option<totally_ordered>>);
    static_assert(has_less<opt::option<totally_ordered>, opt::option<int>>);

    static_assert(!has_less<opt::option<non_comparable>>);
    static_assert(!has_less<opt::option<equality_comparable>>);
    static_assert(!has_less<opt::option<totally_ordered>, opt::option<non_comparable>>);

    struct X {
        int value;
        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator<(const X& x, const X& y) {
        return x.value < y.value;
    }

    constexpr bool test() {
        {
            const opt::option<X> o1;
            const opt::option<X> o2;
            const opt::option<X> o3{1};
            const opt::option<X> o4{2};
            const opt::option<X> o5{1};

            check_meta_auto(o1 < o1, false, !noexcept);
            check_meta_auto(o1 < o2, false, !noexcept);
            check_meta_auto(o1 < o3, true, !noexcept);
            check_meta_auto(o1 < o4, true, !noexcept);
            check_meta_auto(o1 < o5, true, !noexcept);

            check_meta_auto(o2 < o1, false, !noexcept);
            check_meta_auto(o2 < o2, false, !noexcept);
            check_meta_auto(o2 < o3, true, !noexcept);
            check_meta_auto(o2 < o4, true, !noexcept);
            check_meta_auto(o2 < o5, true, !noexcept);

            check_meta_auto(o3 < o1, false, !noexcept);
            check_meta_auto(o3 < o2, false, !noexcept);
            check_meta_auto(o3 < o3, false, !noexcept);
            check_meta_auto(o3 < o4, true, !noexcept);
            check_meta_auto(o3 < o5, false, !noexcept);

            check_meta_auto(o4 < o1, false, !noexcept);
            check_meta_auto(o4 < o2, false, !noexcept);
            check_meta_auto(o4 < o3, false, !noexcept);
            check_meta_auto(o4 < o4, false, !noexcept);
            check_meta_auto(o4 < o5, false, !noexcept);

            check_meta_auto(o5 < o1, false, !noexcept);
            check_meta_auto(o5 < o2, false, !noexcept);
            check_meta_auto(o5 < o3, false, !noexcept);
            check_meta_auto(o5 < o4, true, !noexcept);
            check_meta_auto(o5 < o5, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 < opt::option<long>{101}, true, !noexcept);
            check_meta_auto(opt::option<long>{101} < o1, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 < opt::option<const int>{101}, true, !noexcept);
            check_meta_auto(opt::option<const int>{101} < o1, false, !noexcept);
        }

        return true;
    }
} // namespace less

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.relops/less_equal.pass.cpp
namespace less_equal {
    static_assert(has_less_equal<opt::option<totally_ordered>>);
    static_assert(has_less_equal<opt::option<totally_ordered>, opt::option<int>>);

    static_assert(!has_less_equal<opt::option<non_comparable>>);
    static_assert(!has_less_equal<opt::option<equality_comparable>>);
    static_assert(!has_less_equal<opt::option<totally_ordered>, opt::option<non_comparable>>);

    struct X {
        int value;
        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator<=(const X& x, const X& y) {
        return x.value <= y.value;
    }

    constexpr bool test() {
        {
            const opt::option<X> o1;
            const opt::option<X> o2;
            const opt::option<X> o3{1};
            const opt::option<X> o4{2};
            const opt::option<X> o5{1};

            check_meta_auto(o1 <= o1, true, !noexcept);
            check_meta_auto(o1 <= o2, true, !noexcept);
            check_meta_auto(o1 <= o3, true, !noexcept);
            check_meta_auto(o1 <= o4, true, !noexcept);
            check_meta_auto(o1 <= o5, true, !noexcept);

            check_meta_auto(o2 <= o1, true, !noexcept);
            check_meta_auto(o2 <= o2, true, !noexcept);
            check_meta_auto(o2 <= o3, true, !noexcept);
            check_meta_auto(o2 <= o4, true, !noexcept);
            check_meta_auto(o2 <= o5, true, !noexcept);

            check_meta_auto(o3 <= o1, false, !noexcept);
            check_meta_auto(o3 <= o2, false, !noexcept);
            check_meta_auto(o3 <= o3, true, !noexcept);
            check_meta_auto(o3 <= o4, true, !noexcept);
            check_meta_auto(o3 <= o5, true, !noexcept);

            check_meta_auto(o4 <= o1, false, !noexcept);
            check_meta_auto(o4 <= o2, false, !noexcept);
            check_meta_auto(o4 <= o3, false, !noexcept);
            check_meta_auto(o4 <= o4, true, !noexcept);
            check_meta_auto(o4 <= o5, false, !noexcept);

            check_meta_auto(o5 <= o1, false, !noexcept);
            check_meta_auto(o5 <= o2, false, !noexcept);
            check_meta_auto(o5 <= o3, true, !noexcept);
            check_meta_auto(o5 <= o4, true, !noexcept);
            check_meta_auto(o5 <= o5, true, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 <= opt::option<long>{42}, true, !noexcept);
            check_meta_auto(opt::option<long>{101} <= o1, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 <= opt::option<const int>{42}, true, !noexcept);
            check_meta_auto(opt::option<const int>{101} <= o1, false, !noexcept);
        }

        return true;
    }
} // namespace less_equal

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.relops/not_equal.pass.cpp
namespace not_equal {
    static_assert(has_not_equal<opt::option<int>>);
    static_assert(has_not_equal<opt::option<equality_comparable>>);
    static_assert(has_not_equal<opt::option<equality_comparable>, opt::option<int>>);

    static_assert(!has_not_equal<opt::option<non_comparable>>);
    static_assert(!has_not_equal<opt::option<equality_comparable>, opt::option<non_comparable>>);

    struct X {
        int value;
        constexpr X(int v) : value{v} {}
    };
    constexpr bool operator!=(const X& x, const X& y) {
        return x.value != y.value;
    }

    constexpr bool test() {
        {
            const opt::option<X> o1;
            const opt::option<X> o2;
            const opt::option<X> o3{1};
            const opt::option<X> o4{2};
            const opt::option<X> o5{1};

            check_meta_auto(o1 != o1, false, !noexcept);
            check_meta_auto(o1 != o2, false, !noexcept);
            check_meta_auto(o1 != o3, true, !noexcept);
            check_meta_auto(o1 != o4, true, !noexcept);
            check_meta_auto(o1 != o5, true, !noexcept);

            check_meta_auto(o2 != o1, false, !noexcept);
            check_meta_auto(o2 != o2, false, !noexcept);
            check_meta_auto(o2 != o3, true, !noexcept);
            check_meta_auto(o2 != o4, true, !noexcept);
            check_meta_auto(o2 != o5, true, !noexcept);

            check_meta_auto(o3 != o1, true, !noexcept);
            check_meta_auto(o3 != o2, true, !noexcept);
            check_meta_auto(o3 != o3, false, !noexcept);
            check_meta_auto(o3 != o4, true, !noexcept);
            check_meta_auto(o3 != o5, false, !noexcept);

            check_meta_auto(o4 != o1, true, !noexcept);
            check_meta_auto(o4 != o2, true, !noexcept);
            check_meta_auto(o4 != o3, true, !noexcept);
            check_meta_auto(o4 != o4, false, !noexcept);
            check_meta_auto(o4 != o5, true, !noexcept);

            check_meta_auto(o5 != o1, true, !noexcept);
            check_meta_auto(o5 != o2, true, !noexcept);
            check_meta_auto(o5 != o3, false, !noexcept);
            check_meta_auto(o5 != o4, true, !noexcept);
            check_meta_auto(o5 != o5, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 != opt::option<long>{101}, true, !noexcept);
            check_meta_auto(opt::option<long>{42} != o1, false, !noexcept);
        }
        {
            const opt::option<int> o1{42};
            check_meta_auto(o1 != opt::option<const int>{101}, true, !noexcept);
            check_meta_auto(opt::option<const int>{42} != o1, false, !noexcept);
        }

        return true;
    }
} // namespace not_equal

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.relops/compare.three_way.pass.cpp
namespace three_way {
    static_assert(has_spaceship<opt::option<int>>);
    static_assert(has_spaceship<opt::option<three_way_comparable>>);
    static_assert(has_spaceship<opt::option<strong_order>>);

    static_assert(!has_spaceship<opt::option<non_comparable>>);
    static_assert(!has_spaceship<opt::option<equality_comparable>>);
    static_assert(!has_spaceship<opt::option<totally_ordered>>);

    static_assert(has_spaceship<opt::option<int>, opt::option<long>>);
    static_assert(has_spaceship<opt::option<three_way_comparable>, opt::option<int>>);
    static_assert(!has_spaceship<opt::option<three_way_comparable>, opt::option<non_comparable>>);

    constexpr bool test() {
        {
            const opt::option<int> o1;
            const opt::option<int> o2;

            check_meta(o1 <=> o2, std::strong_ordering, std::strong_ordering::equal, !noexcept);
            check_meta_auto(test_order(o1, o2, std::strong_ordering::equal), true, !noexcept);
        }
        {
            const opt::option<int> o1{3};
            const opt::option<int> o2{3};
            check_meta(o1 <=> o1, std::strong_ordering, std::strong_ordering::equal, !noexcept);
            check_meta_auto(test_order(o1, o1, std::strong_ordering::equal), true, !noexcept);
            check_meta(o1 <=> o2, std::strong_ordering, std::strong_ordering::equal, !noexcept);
            check_meta_auto(test_order(o1, o2, std::strong_ordering::equal), true, !noexcept);
            check_meta(o2 <=> o1, std::strong_ordering, std::strong_ordering::equal, !noexcept);
            check_meta_auto(test_order(o2, o1, std::strong_ordering::equal), true, !noexcept);
        }
        {
            const opt::option<int> o1;
            const opt::option<int> o2{2};
            const opt::option<int> o3{3};
            check_meta(o1 <=> o3, std::strong_ordering, std::strong_ordering::less, !noexcept);
            check_meta_auto(test_order(o1, o3, std::strong_ordering::less), true, !noexcept);
            check_meta(o2 <=> o3, std::strong_ordering, std::strong_ordering::less, !noexcept);
            check_meta_auto(test_order(o2, o3, std::strong_ordering::less), true, !noexcept);
        }
        {
            const opt::option<int> o1;
            const opt::option<int> o2{3};
            const opt::option<int> o3{2};
            check_meta(o2 <=> o1, std::strong_ordering, std::strong_ordering::greater, !noexcept);
            check_meta_auto(test_order(o2, o1, std::strong_ordering::greater), true, !noexcept);
            check_meta(o2 <=> o3, std::strong_ordering, std::strong_ordering::greater, !noexcept);
            check_meta_auto(test_order(o2, o3, std::strong_ordering::greater), true, !noexcept);
        }

        return true;
    }
} // namespace three_way

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.relops/relops.compile.pass.cpp
namespace lwg4370 {
    // https://cplusplus.github.io/LWG/issue4370
    // LWG 4370: comparison operators of option must accept an underlying
    // return type that is convertible both from and to bool, not merely
    // convertible to bool. `Bool` exercises both directions via
    // Bool(bool) (from bool) and operator bool() (to bool).
    struct Bool {
        constexpr Bool(bool) {}
        constexpr operator bool() const { return true; }
    };

    struct S {
        constexpr Bool operator==(S) const { return true; }
        constexpr Bool operator!=(S) const { return true; }
        constexpr Bool operator<=(S) const { return true; }
        constexpr Bool operator<(S) const { return true; }
        constexpr Bool operator>(S) const { return true; }
        constexpr Bool operator>=(S) const { return true; }
    };

    static_assert(has_equal<opt::option<S>>);
    static_assert(has_not_equal<opt::option<S>>);
    static_assert(has_less<opt::option<S>>);
    static_assert(has_greater<opt::option<S>>);
    static_assert(has_less_equal<opt::option<S>>);
    static_assert(has_greater_equal<opt::option<S>>);

    static_assert(has_equal<opt::option<S>, S>);
    static_assert(has_not_equal<opt::option<S>, S>);
    static_assert(has_less<opt::option<S>, S>);
    static_assert(has_greater<opt::option<S>, S>);
    static_assert(has_less_equal<opt::option<S>, S>);
    static_assert(has_greater_equal<opt::option<S>, S>);

    static_assert(has_equal<S, opt::option<S>>);
    static_assert(has_not_equal<S, opt::option<S>>);
    static_assert(has_less<S, opt::option<S>>);
    static_assert(has_greater<S, opt::option<S>>);
    static_assert(has_less_equal<S, opt::option<S>>);
    static_assert(has_greater_equal<S, opt::option<S>>);

    constexpr bool test() {
        const opt::option<S> s{S{}};

        check_meta_auto(s == S{}, true, !noexcept);
        check_meta_auto(s != S{}, true, !noexcept);
        check_meta_auto(s < S{}, true, !noexcept);
        check_meta_auto(s > S{}, true, !noexcept);
        check_meta_auto(s <= S{}, true, !noexcept);
        check_meta_auto(s >= S{}, true, !noexcept);

        return true;
    }
} // namespace lwg4370

}

void relops_test_suite() {
    static_assert(equal::test());
    equal::test();

    static_assert(greater::test());
    greater::test();

    static_assert(greater_equal::test());
    greater_equal::test();

    static_assert(less::test());
    less::test();

    static_assert(less_equal::test());
    less_equal::test();

    static_assert(not_equal::test());
    not_equal::test();

    static_assert(three_way::test());
    three_way::test();

    static_assert(lwg4370::test());
    lwg4370::test();
}
