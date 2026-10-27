
#include <opt/option.hpp>
#include <type_traits>
#include <algorithm>
#include <concepts>
#include <optional>
#include <ranges>
#include <array>

#include <tests.hpp>

namespace {
constexpr bool test_ctor() {
    [[maybe_unused]] opt::none_t abc{opt::none};
    return true;
}

static_assert(std::is_empty_v<opt::none_t>);
static_assert(!std::is_default_constructible_v<opt::none_t>);
static_assert(std::is_same_v<const opt::none_t, decltype(opt::none)>);

static_assert(std::copyable<opt::none_t>);
static_assert(std::three_way_comparable<opt::none_t, std::strong_ordering>);

constexpr bool test() {
    check_meta(opt::none == opt::none, bool, true, noexcept);
    check_meta(opt::none != opt::none, bool, false, noexcept);
    check_meta(opt::none <= opt::none, bool, true, noexcept);
    check_meta(opt::none >= opt::none, bool, true, noexcept);
    check_meta(opt::none > opt::none, bool, false, noexcept);
    check_meta(opt::none < opt::none, bool, false, noexcept);
    check_meta(opt::none <=> opt::none, std::strong_ordering, std::strong_ordering::equal, noexcept);

    { // test std::ranges::find with none
        std::array<opt::option<int>, 5> vec = {1, 2, opt::none, 4, 5};

        auto it = std::ranges::find(vec, opt::none);
        check(it != vec.end());
        check(*it == opt::none);
    }

    return true;
}

}

void none_test_suite() {
    test_ctor();
    static_assert(test_ctor());

    test();
    static_assert(test());
}
