#pragma once

#include <type_traits>

// https://github.com/llvm/llvm-project/blob/b9253b904b7f42d6ecdfdb551dfd67b5e10f476c/libcxx/test/support/test_comparisons.h#L36-L96
template <class T, class U = T>
constexpr bool test_comparisons_complete(const T& t1, const U& t2, bool is_equal, bool is_less, bool is_greater) {
    check((is_equal ? 1 : 0) + (is_less ? 1 : 0) + (is_greater ? 1 : 0) <= 1); // at most one of isEqual, isLess, and isGreater can be true
    if (is_equal) {
        if (!(t1 == t2)) { return false; }
        if (!(t2 == t1)) { return false; }
        if ( (t1 != t2)) { return false; }
        if ( (t2 != t1)) { return false; }
        if ( (t1  < t2)) { return false; }
        if ( (t2  < t1)) { return false; }
        if (!(t1 <= t2)) { return false; }
        if (!(t2 <= t1)) { return false; }
        if ( (t1  > t2)) { return false; }
        if ( (t2  > t1)) { return false; }
        if (!(t1 >= t2)) { return false; }
        if (!(t2 >= t1)) { return false; }
    } else if (is_less) {
        if ( (t1 == t2)) { return false; }
        if ( (t2 == t1)) { return false; }
        if (!(t1 != t2)) { return false; }
        if (!(t2 != t1)) { return false; }
        if (!(t1  < t2)) { return false; }
        if ( (t2  < t1)) { return false; }
        if (!(t1 <= t2)) { return false; }
        if ( (t2 <= t1)) { return false; }
        if ( (t1  > t2)) { return false; }
        if (!(t2  > t1)) { return false; }
        if ( (t1 >= t2)) { return false; }
        if (!(t2 >= t1)) { return false; }
    } else if (is_greater) {
        if ( (t1 == t2)) { return false; }
        if ( (t2 == t1)) { return false; }
        if (!(t1 != t2)) { return false; }
        if (!(t2 != t1)) { return false; }
        if ( (t1  < t2)) { return false; }
        if (!(t2  < t1)) { return false; }
        if ( (t1 <= t2)) { return false; }
        if (!(t2 <= t1)) { return false; }
        if (!(t1  > t2)) { return false; }
        if ( (t2  > t1)) { return false; }
        if (!(t1 >= t2)) { return false; }
        if ( (t2 >= t1)) { return false; }
    } else { // unordered
        if ( (t1 == t2)) { return false; }
        if ( (t2 == t1)) { return false; }
        if (!(t1 != t2)) { return false; }
        if (!(t2 != t1)) { return false; }
        if ( (t1  < t2)) { return false; }
        if ( (t2  < t1)) { return false; }
        if ( (t1 <= t2)) { return false; }
        if ( (t2 <= t1)) { return false; }
        if ( (t1  > t2)) { return false; }
        if ( (t2  > t1)) { return false; }
        if ( (t1 >= t2)) { return false; }
        if ( (t2 >= t1)) { return false; }
    }
    return true;
}

// https://github.com/llvm/llvm-project/blob/b9253b904b7f42d6ecdfdb551dfd67b5e10f476c/libcxx/test/support/test_comparisons.h#L161-L168
template<typename Order, typename T, typename U = T>
constexpr bool test_order(const T& t1, const U& t2, Order order) {
    bool equal = order == Order::equivalent;
    bool less = order == Order::less;
    bool greater = order == Order::greater;

    return ((t1 <=> t2) == order) && test_comparisons_complete(t1, t2, equal, less, greater);
}


template<typename T, typename U = T>
concept has_equal = requires(T t, U u) { t == u; };
template<typename T, typename U = T>
concept has_not_equal = requires(T t, U u) { t != u; };
template<typename T, typename U = T>
concept has_less = requires(T t, U u) { t < u; };
template<typename T, typename U = T>
concept has_less_equal = requires(T t, U u) { t <= u; };
template<typename T, typename U = T>
concept has_greater = requires(T t, U u) { t > u; };
template<typename T, typename U = T>
concept has_greater_equal = requires(T t, U u) { t >= u; };
template<typename T, typename U = T>
concept has_spaceship = requires(T t, U u) { t <=> u; };

struct non_comparable {};

static_assert(!has_equal<non_comparable>);
static_assert(!has_greater<non_comparable>);
static_assert(!has_greater_equal<non_comparable>);
static_assert(!has_less<non_comparable>);
static_assert(!has_less_equal<non_comparable>);
static_assert(!has_not_equal<non_comparable>);

static_assert(!std::equality_comparable<non_comparable>);
static_assert(!std::totally_ordered<non_comparable>);
static_assert(!std::three_way_comparable<non_comparable>);
static_assert(!has_spaceship<non_comparable>);

struct equality_comparable {
    int value;
    [[maybe_unused]] constexpr equality_comparable(int v) : value{v} {} // implicit

    [[maybe_unused]] friend bool operator==(const equality_comparable&, const equality_comparable&) = default;
};

static_assert(has_equal<equality_comparable>);
static_assert(has_not_equal<equality_comparable>);
static_assert(!has_greater<equality_comparable>);
static_assert(!has_greater_equal<equality_comparable>);
static_assert(!has_less<equality_comparable>);
static_assert(!has_less_equal<equality_comparable>);

static_assert(std::equality_comparable<equality_comparable>);
static_assert(!std::totally_ordered<equality_comparable>);
static_assert(!std::three_way_comparable<equality_comparable>);
static_assert(!has_spaceship<equality_comparable>);

struct totally_ordered {
    int value;

    [[maybe_unused]] constexpr totally_ordered(int v) : value{v} {}

    [[maybe_unused]] friend bool operator==(const totally_ordered&, const totally_ordered&) = default;

    [[maybe_unused]] friend constexpr bool operator<(const totally_ordered& x, const totally_ordered& y) noexcept {
        return x.value < y.value;
    }
    [[maybe_unused]] friend constexpr bool operator>(const totally_ordered& x, const totally_ordered& y) noexcept {
        return x.value > y.value;
    }
    [[maybe_unused]] friend constexpr bool operator<=(const totally_ordered& x, const totally_ordered& y) noexcept {
        return x.value <= y.value;
    }
    [[maybe_unused]] friend constexpr bool operator>=(const totally_ordered& x, const totally_ordered& y) noexcept {
        return x.value >= y.value;
    }
};

static_assert(has_equal<totally_ordered>);
static_assert(has_not_equal<totally_ordered>);
static_assert(has_greater<totally_ordered>);
static_assert(has_greater_equal<totally_ordered>);
static_assert(has_less<totally_ordered>);
static_assert(has_less_equal<totally_ordered>);

static_assert(std::equality_comparable<totally_ordered>);
static_assert(std::totally_ordered<totally_ordered>);
static_assert(!std::three_way_comparable<totally_ordered>);
static_assert(!has_spaceship<totally_ordered>);

struct strong_order {
    int value;
    [[maybe_unused]] constexpr strong_order(int v) : value{v} {}
    [[maybe_unused]] friend std::strong_ordering operator<=>(const strong_order&, const strong_order&) = default;
};

struct weak_order {
    int value;
    [[maybe_unused]] constexpr weak_order(int v) : value{v} {}
    [[maybe_unused]] friend std::weak_ordering operator<=>(const weak_order&, const weak_order&) = default;
};

struct partial_order {
    int value;
    [[maybe_unused]] constexpr partial_order(int v) : value{v} {}

    friend constexpr std::partial_ordering operator<=>(const partial_order& x, const partial_order& y) {
        if (x.value == std::numeric_limits<int>::min() || y.value == std::numeric_limits<int>::min()) {
            return std::partial_ordering::unordered;
        }
        if (x.value == std::numeric_limits<int>::max() || y.value == std::numeric_limits<int>::max()) {
            return std::partial_ordering::unordered;
        }
        return x.value <=> y.value;
    }
    [[maybe_unused]] friend constexpr bool operator==(const partial_order& x, const partial_order& y) {
        return (x <=> y) == std::partial_ordering::equivalent;
    }
};

struct three_way_comparable {
    int value;
    [[maybe_unused]] constexpr three_way_comparable(int v) : value{v} {}

    [[maybe_unused]] friend constexpr bool operator==(const three_way_comparable&, const three_way_comparable&) = default;
    [[maybe_unused]] friend constexpr std::strong_ordering operator<=>(const three_way_comparable&, const three_way_comparable&) = default;
};

static_assert(std::equality_comparable<three_way_comparable>);
static_assert(std::three_way_comparable<three_way_comparable>);
static_assert(has_equal<three_way_comparable>);
static_assert(has_greater<three_way_comparable>);
static_assert(has_greater_equal<three_way_comparable>);
static_assert(has_less<three_way_comparable>);
static_assert(has_less_equal<three_way_comparable>);
static_assert(has_not_equal<three_way_comparable>);
static_assert(has_spaceship<three_way_comparable>);
