
#include <opt/option.hpp>
#include <tests.hpp>

namespace {

// https://github.com/llvm/llvm-project/blob/main/libcxx/test/std/utilities/optional/optional.object/optional.object.observe/op_arrow.pass.cpp

struct X {
    constexpr int test() noexcept { return 3; }
    constexpr int test() const noexcept { return 4; }
};

template<typename T>
constexpr void test_contract() {
    opt::option<T> opt;
    check_only_meta(opt.operator->(), T*, noexcept);
    check_only_meta(std::as_const(opt).operator->(), const T*, noexcept);
}

// TODO: test_ref

constexpr bool test() {
    test_contract<int>();
    test_contract<float>();
    test_contract<double>();
    test_contract<X>();

    test_contract<const int>();
    test_contract<const float>();
    test_contract<const double>();
    test_contract<const X>();

    opt::option<X> o{X{}};
    check(o->test() == 3);
    check(std::as_const(o)->test() == 4);
    check(o.operator->() == &*o);
    check(std::as_const(o).operator->() == &*o);

    // TODO: test_ref

    return true;
}

}

void op_arrow_test_suite() {
    test();
    static_assert(test());
}
