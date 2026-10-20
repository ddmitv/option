
#include <opt/option.hpp>

#include <tests.hpp>
#include <comp_ops/comparison_helpers.hpp>

namespace {

constexpr bool test() {
    const opt::option<int> o1;
    const opt::option<int> o2{1};

    check_meta_auto(opt::none == o1, true, noexcept);
    check_meta_auto(opt::none == o2, false, noexcept);
    check_meta_auto(o1 == opt::none, true, noexcept);
    check_meta_auto(o2 == opt::none, false, noexcept);

    check_meta_auto(opt::none > o1, false, noexcept);
    check_meta_auto(opt::none > o2, false, noexcept);
    check_meta_auto(o1 > opt::none, false, noexcept);
    check_meta_auto(o2 > opt::none, true, noexcept);

    check_meta_auto(opt::none >= o1, true, noexcept);
    check_meta_auto(opt::none >= o2, false, noexcept);
    check_meta_auto(o1 >= opt::none, true, noexcept);
    check_meta_auto(o2 >= opt::none, true, noexcept);

    check_meta_auto(opt::none <= o1, true, noexcept);
    check_meta_auto(opt::none <= o2, true, noexcept);
    check_meta_auto(o1 <= opt::none, true, noexcept);
    check_meta_auto(o2 <= opt::none, false, noexcept);

    check_meta_auto(opt::none < o1, false, noexcept);
    check_meta_auto(opt::none < o2, true, noexcept);
    check_meta_auto(o1 < opt::none, false, noexcept);
    check_meta_auto(o2 < opt::none, false, noexcept);

    check_meta_auto(opt::none != o1, false, noexcept);
    check_meta_auto(opt::none != o2, true, noexcept);
    check_meta_auto(o1 != opt::none, false, noexcept);
    check_meta_auto(o2 != opt::none, true, noexcept);

    check_meta(opt::none <=> o1, std::strong_ordering, std::strong_ordering::equal, noexcept);
    check(test_order(opt::none, o1, std::strong_ordering::equal));
    check_meta(o1 <=> opt::none, std::strong_ordering, std::strong_ordering::equal, noexcept);
    check(test_order(o1, opt::none, std::strong_ordering::equal));

    check_meta(opt::none <=> o2, std::strong_ordering, std::strong_ordering::less, noexcept);
    check(test_order(opt::none, o2, std::strong_ordering::less));

    check_meta(o2 <=> opt::none, std::strong_ordering, std::strong_ordering::greater, noexcept);
    check(test_order(o2, opt::none, std::strong_ordering::greater));
    return true;
}

}

void comp_with_none_test_suite() {
    test();
    static_assert(test());
}
