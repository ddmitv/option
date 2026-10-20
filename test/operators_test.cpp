#include <compare>
#include <limits>

#include <opt/option.hpp>

template<typename T1, typename T2 = T1>
concept has_operator_equal = requires(T1 a, T2 b) { a == b; };

template<typename T1, typename T2 = T1>
concept has_operator_greather_than = requires(T1 a, T2 b) { a > b; };

template<typename T1, typename T2 = T1>
concept has_operator_greather_than_equal = requires(T1 a, T2 b) { a >= b; };

template<typename T1, typename T2 = T1>
concept has_operator_less_than = requires(T1 a, T2 b) { a < b; };

template<typename T1, typename T2 = T1>
concept has_operator_less_than_equal = requires(T1 a, T2 b) { a <= b; };

template<typename T1, typename T2 = T1>
concept has_operator_not_equal = requires(T1 a, T2 b) { a != b; };

template<typename T1, typename T2 = T1>
concept has_operator_spaceship = requires(T1 a, T2 b) { a <=> b; };

struct non_comparable {};

class equality_comparable {
public:
    constexpr equality_comparable(int value_) : value{value_} {}

    friend bool operator==(const equality_comparable&, const equality_comparable&) = default;

private:
    int value;
};

class totally_ordered {
public:
    constexpr totally_ordered(int value_) : value{value_} {}

    friend bool operator==(const totally_ordered&, const totally_ordered&) = default;
    friend constexpr bool operator<(const totally_ordered& a, const totally_ordered& b) noexcept {
        return a.value < b.value;
    }
    friend constexpr bool operator>(const totally_ordered& a, const totally_ordered& b) noexcept {
        return a.value > b.value;
    }
    friend constexpr bool operator<=(const totally_ordered& a, const totally_ordered& b) noexcept {
        return a.value <= b.value;
    }
    friend constexpr bool operator>=(const totally_ordered& a, const totally_ordered& b) noexcept {
        return a.value >= b.value;
    }

private:
    int value;
};

class strong_order {
public:
    constexpr strong_order(int v) : value{v} {}
    friend std::strong_ordering operator<=>(const strong_order&, const strong_order&) = default;
private:
    int value;
};

class weak_order {
public:
    constexpr weak_order(int v) : value{v} {}
    friend std::weak_ordering operator<=>(const weak_order&, const weak_order&) = default;
private:
    int value;
};

class partial_order {
public:
    constexpr partial_order(int v) : value{v} {}
    friend constexpr std::partial_ordering operator<=>(const partial_order& lhs, const partial_order& rhs) {
        if (lhs.value == std::numeric_limits<int>::min() || rhs.value == std::numeric_limits<int>::min())
            return std::partial_ordering::unordered;
        if (lhs.value == std::numeric_limits<int>::max() || rhs.value == std::numeric_limits<int>::max())
            return std::partial_ordering::unordered;
        return lhs.value <=> rhs.value;
    }
    friend constexpr bool operator==(const partial_order& lhs, const partial_order& rhs) {
        return (lhs <=> rhs) == std::partial_ordering::equivalent;
    }
private:
    int value;
};

class three_way_comparable {
public:
    constexpr three_way_comparable(int value_) : value{value_} {}

    friend bool operator==(const three_way_comparable&, const three_way_comparable&) = default;
    friend std::strong_ordering operator<=>(const three_way_comparable&, const three_way_comparable&) = default;
private:
    int value;
};

static_assert(has_operator_equal<opt::option<int>>);
static_assert(has_operator_equal<opt::option<equality_comparable>>);
static_assert(has_operator_equal<opt::option<equality_comparable>, opt::option<int>>);
static_assert(!has_operator_equal<opt::option<non_comparable>>);
static_assert(!has_operator_equal<opt::option<equality_comparable>, opt::option<non_comparable>>);
